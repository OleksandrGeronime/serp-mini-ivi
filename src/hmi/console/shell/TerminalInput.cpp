#include "TerminalInput.hpp"

#include <algorithm>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <poll.h>
#include <sys/ioctl.h>
#include <unistd.h>

namespace mini_hmi {
namespace {
TerminalSession* gActiveTerminal = nullptr;
struct sigaction gPreviousSegv{};
struct sigaction gPreviousAbrt{};
struct sigaction gPreviousTerm{};
struct sigaction gPreviousInt{};
bool gSignalsInstalled = false;

void disableTerminalFeatures() noexcept {
    static constexpr const char* reset = "\033[?1006l\033[?1000l\033[?1002l\033[?1003l\033[?25h\033[?1049l\033[0m";
    (void)::write(STDOUT_FILENO, reset, std::strlen(reset));
}

void chainOrDefault(int signal, const struct sigaction& previous) {
    if (previous.sa_handler == SIG_IGN) return;
    if (previous.sa_handler && previous.sa_handler != SIG_DFL) {
        previous.sa_handler(signal);
        return;
    }
    std::signal(signal, SIG_DFL);
    std::raise(signal);
}

void terminalSignalHandler(int signal) {
    disableTerminalFeatures();
    switch (signal) {
        case SIGSEGV: chainOrDefault(signal, gPreviousSegv); break;
        case SIGABRT: chainOrDefault(signal, gPreviousAbrt); break;
        case SIGTERM: chainOrDefault(signal, gPreviousTerm); break;
        case SIGINT:  chainOrDefault(signal, gPreviousInt); break;
        default:
            std::signal(signal, SIG_DFL);
            std::raise(signal);
            break;
    }
}

void installTerminalSignalHandlers() {
    if (gSignalsInstalled) return;
    struct sigaction action{};
    action.sa_handler = terminalSignalHandler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGSEGV, &action, &gPreviousSegv);
    sigaction(SIGABRT, &action, &gPreviousAbrt);
    sigaction(SIGTERM, &action, &gPreviousTerm);
    sigaction(SIGINT, &action, &gPreviousInt);
    gSignalsInstalled = true;
}

void restoreTerminalSignalHandlers() {
    if (!gSignalsInstalled) return;
    sigaction(SIGSEGV, &gPreviousSegv, nullptr);
    sigaction(SIGABRT, &gPreviousAbrt, nullptr);
    sigaction(SIGTERM, &gPreviousTerm, nullptr);
    sigaction(SIGINT, &gPreviousInt, nullptr);
    gSignalsInstalled = false;
}

bool isKnownEscapePrefix(const std::string& buffer) {
    return buffer == "\033" || buffer == "\033[" || buffer == "\033[<" || buffer == "\033[M";
}
} // namespace

TerminalSession::TerminalSession() {
    gActiveTerminal = this;
    installTerminalSignalHandlers();
    if (tcgetattr(STDIN_FILENO, &mOriginal) == 0) {
        mHasOriginal = true;
        termios raw = mOriginal;
        raw.c_lflag &= static_cast<unsigned>(~(ICANON | ECHO));
        raw.c_iflag &= static_cast<unsigned>(~(IXON | ICRNL));
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &raw);
    }
    std::printf("\033[?1049h\033[?25l\033[?1000h\033[?1002h\033[?1006h\033[2J\033[H");
    std::fflush(stdout);
}

TerminalSession::~TerminalSession() { restore(); }

void TerminalSession::restore() noexcept {
    if (mRestored) return;
    mRestored = true;
    disableTerminalFeatures();
    if (mHasOriginal) tcsetattr(STDIN_FILENO, TCSANOW, &mOriginal);
    if (gActiveTerminal == this) gActiveTerminal = nullptr;
    restoreTerminalSignalHandlers();
}

void TerminalSession::draw(const std::string& frame) {
    // Renderer fills the whole terminal area with a stable-size ASCII frame.
    // Cursor-home redraw avoids the bottom-half flicker caused by clear-after-draw.
    std::printf("\033[H%s", frame.c_str());
    std::fflush(stdout);
}

int TerminalSession::width() const {
    winsize size{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0 && size.ws_col > 0) return size.ws_col;
    return 120;
}

int TerminalSession::height() const {
    winsize size{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == 0 && size.ws_row > 0) return size.ws_row;
    return 36;
}

std::optional<InputEvent> TerminalSession::pollEvent(int timeoutMs) {
    if (auto event = parseBuffer()) return event;

    pollfd descriptor{};
    descriptor.fd = STDIN_FILENO;
    descriptor.events = POLLIN;
    const int result = poll(&descriptor, 1, timeoutMs);
    if (result <= 0 || !(descriptor.revents & POLLIN)) return std::nullopt;

    char raw[256];
    const ssize_t n = ::read(STDIN_FILENO, raw, sizeof(raw));
    if (n <= 0) return std::nullopt;
    mBuffer.append(raw, static_cast<std::size_t>(n));
    return parseBuffer();
}

bool TerminalSession::parseSgrMouse(InputEvent& event) {
    if (mBuffer.rfind("\033[<", 0) != 0) return false;
    const auto terminator = mBuffer.find_first_of("mM");
    if (terminator == std::string::npos) return false;

    const std::string sequence = mBuffer.substr(0, terminator + 1);
    mBuffer.erase(0, terminator + 1);

    int state = 0;
    int x = 0;
    int y = 0;
    char suffix = 0;
    if (std::sscanf(sequence.c_str(), "\033[<%d;%d;%d%c", &state, &x, &y, &suffix) != 4) {
        return true;
    }

    const bool motion = (state & 32) != 0;
    const bool wheel = (state & 64) != 0;
    const int button = state & 3;
    // Treat both press (M) and release (m) as valid click candidates. Terminals
    // differ here; button hit-testing is idempotent enough for this demo.
    if (motion || wheel || button != 0) return true;

    event.type = InputEvent::Type::MouseClick;
    event.mouseX = std::max(0, x - 1);
    event.mouseY = std::max(0, y - 1);
    return true;
}

bool TerminalSession::parseX10Mouse(InputEvent& event) {
    if (mBuffer.rfind("\033[M", 0) != 0) return false;
    if (mBuffer.size() < 6) return false;

    const unsigned char state = static_cast<unsigned char>(mBuffer[3]);
    const unsigned char x = static_cast<unsigned char>(mBuffer[4]);
    const unsigned char y = static_cast<unsigned char>(mBuffer[5]);
    mBuffer.erase(0, 6);

    const bool wheel = (state & 64) != 0;
    if (wheel) return true;
    event.type = InputEvent::Type::MouseClick;
    event.mouseX = std::max(0, static_cast<int>(x) - 33);
    event.mouseY = std::max(0, static_cast<int>(y) - 33);
    return true;
}

std::optional<InputEvent> TerminalSession::parseBuffer() {
    while (!mBuffer.empty()) {
        InputEvent mouse{};
        if (mBuffer.rfind("\033[<", 0) == 0) {
            if (mBuffer.find_first_of("mM") == std::string::npos) return std::nullopt;
            if (parseSgrMouse(mouse)) {
                if (mouse.type == InputEvent::Type::MouseClick) return mouse;
                continue;
            }
        }
        if (mBuffer.rfind("\033[M", 0) == 0) {
            if (mBuffer.size() < 6) return std::nullopt;
            if (parseX10Mouse(mouse)) {
                if (mouse.type == InputEvent::Type::MouseClick) return mouse;
                continue;
            }
        }

        if (isKnownEscapePrefix(mBuffer)) return std::nullopt;
        if (mBuffer.rfind("\033[D", 0) == 0) { mBuffer.erase(0, 3); return InputEvent{InputEvent::Type::Left}; }
        if (mBuffer.rfind("\033[C", 0) == 0) { mBuffer.erase(0, 3); return InputEvent{InputEvent::Type::Right}; }
        if (mBuffer.rfind("\033[A", 0) == 0) { mBuffer.erase(0, 3); return InputEvent{InputEvent::Type::Up}; }
        if (mBuffer.rfind("\033[B", 0) == 0) { mBuffer.erase(0, 3); return InputEvent{InputEvent::Type::Down}; }

        if (mBuffer.front() == '\033') {
            mBuffer.erase(0, 1);
            continue;
        }

        const unsigned char ch = static_cast<unsigned char>(mBuffer.front());
        mBuffer.erase(0, 1);
        if (ch == 'q' || ch == 'Q') return InputEvent{InputEvent::Type::Quit};
        if ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-') { InputEvent e{InputEvent::Type::Digit}; e.digit = static_cast<char>(ch); return e; }
        if (ch == 'd' || ch == 'D') return InputEvent{InputEvent::Type::DriverSeat};
        if (ch == 'c' || ch == 'C') return InputEvent{InputEvent::Type::CoDriverSeat};
        if (ch == 'h' || ch == 'H') return InputEvent{InputEvent::Type::Left};
        if (ch == 'l' || ch == 'L') return InputEvent{InputEvent::Type::Right};
        if (ch == 'j' || ch == 'J') return InputEvent{InputEvent::Type::Down};
        if (ch == 'k' || ch == 'K') return InputEvent{InputEvent::Type::Up};
        if (ch == 'r' || ch == 'R') return InputEvent{InputEvent::Type::Refresh};
        if (ch == '\n' || ch == '\r') return InputEvent{InputEvent::Type::Enter};
    }
    return std::nullopt;
}

} // namespace mini_hmi
