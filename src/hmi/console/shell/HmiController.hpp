#pragma once

#include "HmiClient.hpp"
#include "HmiTypes.hpp"

#include <string>
#include <vector>

namespace mini_hmi {

class HmiController {
public:
    explicit HmiController(IHmiClient& client);

    ViewModel viewModel();
    std::string pressButton(const std::string& buttonId);
    std::string switchScreen(Screen screen);
    std::string switchSeat(Seat seat);
    void runScriptedDemo(std::vector<ViewModel>& frames);

private:
    std::string applyAction(const HmiAction& action);
    void updateStatus();

    IHmiClient& mClient;
    Screen mScreen{Screen::Home};
    Seat mSeat{Seat::Driver};
    ScreenData mState;
    std::string mLastCommand;
    std::string mMessage{"Ready. Use mouse or number keys; Q exits."};
};

} // namespace mini_hmi
