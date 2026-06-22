#pragma once

#include "HmiModules.hpp"
#include "HmiScreens.hpp"

#include <algorithm>

namespace mini_hmi {
namespace module_ui {

inline void field(Canvas& c, int x, int y, const std::string& name, const std::string& value) {
    c.text(x, y, clampText(name + ": " + value, 34));
}

inline void card(Canvas& c, Rect r, const std::string& titleText, const std::vector<std::string>& lines) {
    c.box(r, titleText);
    int y = r.y + 2;
    for (const auto& line : lines) {
        if (y >= r.y + r.h - 1) break;
        c.text(r.x + 2, y++, clampText(line, r.w - 4));
    }
}

inline int boundedTemp(const std::string& value, int fallback) {
    return std::clamp(parseIntLike(value, fallback), 60, 85);
}

} // namespace module_ui
} // namespace mini_hmi
