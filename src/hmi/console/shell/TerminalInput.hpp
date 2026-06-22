#pragma once

#include <optional>
#include <string>
#include <termios.h>

namespace mini_hmi {

struct InputEvent {
    enum class Type { None, Quit, Digit, DriverSeat, CoDriverSeat, Left, Right, Up, Down, Enter, MouseClick, Refresh } type{Type::None};
    char digit{0};
    int mouseX{0};
    int mouseY{0};
};

class TerminalSession {
public:
    TerminalSession();
    ~TerminalSession();
    TerminalSession(const TerminalSession&) = delete;
    TerminalSession& operator=(const TerminalSession&) = delete;

    std::optional<InputEvent> pollEvent(int timeoutMs = 120);
    void draw(const std::string& frame);
    int width() const;
    int height() const;

private:
    std::optional<InputEvent> parseBuffer();
    bool parseSgrMouse(InputEvent& event);
    bool parseX10Mouse(InputEvent& event);
    void restore() noexcept;

    termios mOriginal{};
    bool mHasOriginal{false};
    bool mRestored{false};
    std::string mBuffer;
};

} // namespace mini_hmi
