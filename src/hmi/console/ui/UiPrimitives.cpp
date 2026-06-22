#include "UiPrimitives.hpp"

#include "HmiModules.hpp"

#include <algorithm>
#include <cstdlib>
#include <sstream>

namespace mini_hmi {

std::string clampText(std::string value, int width) {
    if (width <= 0) return {};
    for (char& ch : value) {
        if (static_cast<unsigned char>(ch) < 32 || ch == 127) ch = ' ';
    }
    if (static_cast<int>(value.size()) > width) {
        if (width == 1) return value.substr(0, 1);
        return value.substr(0, static_cast<std::size_t>(width - 1)) + "~";
    }
    value.append(static_cast<std::size_t>(width - static_cast<int>(value.size())), ' ');
    return value;
}

int parseIntLike(const std::string& value, int fallback) {
    try {
        std::size_t pos = 0;
        const int parsed = std::stoi(value, &pos);
        (void)pos;
        return parsed;
    } catch (...) {
        return fallback;
    }
}

int parsePercentLike(const std::string& value, int fallback) {
    return std::clamp(parseIntLike(value, fallback), 0, 100);
}

Canvas::Canvas(int width, int height)
    : mWidth(std::max(96, width)),
      mHeight(std::max(30, height)),
      mRows(static_cast<std::size_t>(mHeight), std::string(static_cast<std::size_t>(mWidth), ' ')) {}

void Canvas::put(int x, int y, char ch) {
    if (x < 0 || y < 0 || x >= mWidth || y >= mHeight) return;
    mRows[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)] = ch;
}

void Canvas::text(int x, int y, const std::string& value) {
    if (y < 0 || y >= mHeight || x >= mWidth) return;
    int xx = std::max(0, x);
    std::size_t offset = x < 0 ? static_cast<std::size_t>(-x) : 0;
    while (offset < value.size() && xx < mWidth) {
        const char ch = value[offset++];
        // Do not let embedded control characters corrupt the terminal frame.
        if (ch >= 32 && ch != 127) put(xx++, y, ch);
    }
}

void Canvas::centered(int y, const std::string& value) {
    text(std::max(0, (mWidth - static_cast<int>(value.size())) / 2), y, value);
}

void Canvas::hline(int x, int y, int width, char ch) {
    for (int i = 0; i < width; ++i) put(x + i, y, ch);
}

void Canvas::vline(int x, int y, int height, char ch) {
    for (int i = 0; i < height; ++i) put(x, y + i, ch);
}

void Canvas::fill(Rect rect, char ch) {
    for (int yy = 0; yy < rect.h; ++yy) {
        for (int xx = 0; xx < rect.w; ++xx) put(rect.x + xx, rect.y + yy, ch);
    }
}

void Canvas::box(Rect rect, const std::string& title) {
    if (rect.w < 2 || rect.h < 2) return;
    put(rect.x, rect.y, '+');
    put(rect.x + rect.w - 1, rect.y, '+');
    put(rect.x, rect.y + rect.h - 1, '+');
    put(rect.x + rect.w - 1, rect.y + rect.h - 1, '+');
    hline(rect.x + 1, rect.y, rect.w - 2, '-');
    hline(rect.x + 1, rect.y + rect.h - 1, rect.w - 2, '-');
    vline(rect.x, rect.y + 1, rect.h - 2, '|');
    vline(rect.x + rect.w - 1, rect.y + 1, rect.h - 2, '|');
    if (!title.empty() && rect.w > 6) {
        text(rect.x + 2, rect.y, " " + clampText(title, rect.w - 6) + " ");
    }
}

void Canvas::progress(Rect rect, int percent, const std::string& label) {
    const int value = std::clamp(percent, 0, 100);
    box(rect, label);
    const int inner = std::max(0, rect.w - 4);
    const int filled = inner * value / 100;
    for (int i = 0; i < inner; ++i) put(rect.x + 2 + i, rect.y + rect.h / 2, i < filled ? '#' : '.');
    const std::string suffix = std::to_string(value) + "%";
    text(rect.x + rect.w - static_cast<int>(suffix.size()) - 2, rect.y + rect.h / 2, suffix);
}

void Canvas::addTarget(Rect rect, std::string id) {
    if (rect.w <= 0 || rect.h <= 0 || id.empty()) return;
    mTargets.push_back(HitTarget{rect.x, rect.y, rect.w, rect.h, std::move(id)});
}

void Canvas::button(Rect rect, const std::string& id, const std::string& label, bool enabled) {
    if (rect.w <= 0 || rect.h <= 0) return;
    const std::string left = enabled ? "[" : "(";
    const std::string right = enabled ? "]" : ")";
    const std::string caption = left + id + " " + label + right;
    fill(rect, ' ');
    box(rect, enabled ? "" : "disabled");
    text(rect.x + 2, rect.y + rect.h / 2, clampText(caption, rect.w - 4));
    addTarget(rect, id);
}

void Canvas::appRail(Rect rect, Screen active) {
    box(rect, "APPS");
    int y = rect.y + 2;
    for (const auto& module : hmiModules()) {
        const auto screen = module.screen;
        const bool selected = screen == active;
        const std::string line = std::string(selected ? "> " : "  ") + module.title;
        text(rect.x + 2, y, clampText(line, rect.w - 4));
        addTarget(Rect{rect.x + 1, y, rect.w - 2, 1}, "screen " + module.id);
        y += 2;
        if (y >= rect.y + rect.h - 1) break;
    }
}

void Canvas::statusBar(const ViewModel& model, const VehicleState& vehicle, const AudioFocusState& focus) {
    fill(Rect{0, 0, mWidth, 3}, ' ');
    hline(0, 2, mWidth, '=');
    text(2, 0, "SERP IVI MINI PLATFORM");
    text(28, 0, "Seat: " + toString(model.seat));
    text(46, 0, "App: " + screenTitle(model.screen));
    text(mWidth - 15, 0, "10:24  LTE");
    text(2, 1, "Vehicle " + vehicle.gear + "  " + vehicle.speed + " mph  moving=" + vehicle.moving);
    text(46, 1, "Audio owner=" + focus.owner + " mode=" + focus.mode);
}

void Canvas::softKeyStrip(const ViewModel& model) {
    const int stripY = std::max(0, mHeight - 8);
    fill(Rect{0, stripY, mWidth, mHeight - stripY}, ' ');
    hline(0, stripY, mWidth, '=');
    text(2, stripY + 1, "Soft keys: click/tap buttons or press 1-9/0/+/- | D/C seat | H/L or arrows apps | Q quit");

    const int count = static_cast<int>(std::min<std::size_t>(model.buttons.size(), 12));
    if (count <= 0) return;

    const int gap = 1;
    const int perRow = count > 6 ? 6 : count;
    const int buttonW = std::max(10, (mWidth - 4 - gap * (perRow - 1)) / perRow);
    for (int i = 0; i < count; ++i) {
        const int row = i / 6;
        const int col = i % 6;
        const int x = 2 + col * (buttonW + gap);
        const int y = stripY + 2 + row * 3;
        const auto& spec = model.buttons[static_cast<std::size_t>(i)];
        button(Rect{x, y, buttonW, 3}, spec.id, spec.label, isButtonAllowed(spec, model.seat));
    }
}

std::string Canvas::str() const {
    std::ostringstream out;
    for (std::size_t i = 0; i < mRows.size(); ++i) {
        if (i) out << '\n';
        out << mRows[i];
    }
    return out.str();
}

} // namespace mini_hmi
