#pragma once

#include "HmiActions.hpp"
#include "HmiState.hpp"

#include <string>
#include <vector>

namespace mini_hmi {

enum class Seat { Driver, CoDriver };
enum class Screen { Home, Media, Radio, Navigation, Phone, Climate, Settings, Profile, Vehicle, Notifications,
                    PhoneDialer, PhoneActiveCall, PhoneContactSearch };

struct ButtonSpec {
    std::string id;
    std::string label;
    HmiAction action;
    bool driverAllowed{true};
    bool codriverAllowed{true};
};

struct HitTarget {
    int x{0};
    int y{0};
    int width{0};
    int height{1};
    std::string buttonId;
};

struct ViewModel {
    Screen screen{Screen::Home};
    Seat seat{Seat::Driver};
    ScreenData state;
    std::string lastCommand;
    std::string message;
    std::vector<ButtonSpec> buttons;
};

std::string toString(Seat seat);
std::string screenId(Screen screen);
std::string screenTitle(Screen screen);
Screen parseScreen(const std::string& value);
Seat parseSeat(const std::string& value, Seat fallback);
std::string lowerCopy(std::string value);
bool isButtonAllowed(const ButtonSpec& button, Seat seat);
std::vector<ButtonSpec> buttonsForScreen(Screen screen);

} // namespace mini_hmi
