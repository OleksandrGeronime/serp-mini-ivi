#include "HmiTypes.hpp"

#include "HmiModules.hpp"

#include <algorithm>
#include <cctype>

namespace mini_hmi {

std::string lowerCopy(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return value;
}

std::string toString(Seat seat) { return seat == Seat::Driver ? "driver" : "codriver"; }

std::string screenId(Screen screen) {
    if (const auto* module = hmiModule(screen)) return module->id;
    return "home";
}

std::string screenTitle(Screen screen) {
    if (const auto* module = hmiModule(screen)) return module->title;
    return "HOME";
}

Screen parseScreen(const std::string& value) {
    if (const auto* module = hmiModuleById(value)) return module->screen;
    return Screen::Home;
}

Seat parseSeat(const std::string& value, Seat fallback) {
    const auto v = lowerCopy(value);
    if (v == "driver") return Seat::Driver;
    if (v == "codriver" || v == "passenger") return Seat::CoDriver;
    return fallback;
}

bool isButtonAllowed(const ButtonSpec& button, Seat seat) {
    return seat == Seat::CoDriver ? button.codriverAllowed : button.driverAllowed;
}

std::vector<ButtonSpec> buttonsForScreen(Screen screen) {
    if (const auto* module = hmiModule(screen)) return module->buttons;
    return {};
}

} // namespace mini_hmi
