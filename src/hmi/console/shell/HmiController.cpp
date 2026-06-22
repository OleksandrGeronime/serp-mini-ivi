#include "HmiController.hpp"

#include <exception>

namespace mini_hmi {

HmiController::HmiController(IHmiClient& client)
    : mClient(client) {
    updateStatus();
}

void HmiController::updateStatus() { mState = mClient.snapshot(); }

ViewModel HmiController::viewModel() {
    updateStatus();
    return ViewModel{mScreen, mSeat, mState, mLastCommand, mMessage, buttonsForScreen(mScreen)};
}

std::string HmiController::switchScreen(Screen screen) {
    mScreen = screen;
    mLastCommand = "screen " + screenId(screen);
    mMessage = "Opened " + screenTitle(screen) + " app.";
    updateStatus();
    return mMessage;
}

std::string HmiController::switchSeat(Seat seat) {
    mSeat = seat;
    mLastCommand = "seat " + toString(seat);
    mMessage = "Active HMI context moved to " + toString(seat) + ".";
    updateStatus();
    return mMessage;
}

std::string HmiController::pressButton(const std::string& buttonId) {
    for (const auto& button : buttonsForScreen(mScreen)) {
        if (button.id != buttonId) continue;
        if (!isButtonAllowed(button, mSeat)) {
            mLastCommand = "button " + buttonId;
            mMessage = "Button disabled for " + toString(mSeat) + ".";
            updateStatus();
            return mMessage;
        }
        return applyAction(button.action);
    }
    mLastCommand = "button " + buttonId;
    mMessage = "No such button on " + screenTitle(mScreen) + ".";
    updateStatus();
    return mMessage;
}

std::string HmiController::applyAction(const HmiAction& action) {
    mLastCommand = describeAction(action);
    try {
        if (action.id == "screen.open" && !action.args.empty()) {
            return switchScreen(parseScreen(action.args[0]));
        }
        if (action.id == "seat.switch" && !action.args.empty()) {
            return switchSeat(parseSeat(action.args[0], mSeat));
        }
        mMessage = mClient.dispatch(toString(mSeat), action);
    } catch (const std::exception& ex) {
        mMessage = std::string{"ERROR: "} + ex.what();
    }
    updateStatus();
    return mMessage;
}

void HmiController::runScriptedDemo(std::vector<ViewModel>& frames) {
    frames.push_back(viewModel());
    pressButton("7"); // media scenario from Home
    frames.push_back(viewModel());
    switchScreen(Screen::Media);
    pressButton("3");
    frames.push_back(viewModel());
    switchScreen(Screen::Navigation);
    pressButton("6");
    frames.push_back(viewModel());
    switchScreen(Screen::Phone);
    pressButton("4");
    frames.push_back(viewModel());
    switchScreen(Screen::Settings);
    pressButton("2");
    pressButton("5");
    frames.push_back(viewModel());
    switchScreen(Screen::Vehicle);
    pressButton("7");
    frames.push_back(viewModel());
    switchSeat(Seat::CoDriver);
    switchScreen(Screen::Climate);
    pressButton("3");
    frames.push_back(viewModel());
    switchScreen(Screen::Notifications);
    frames.push_back(viewModel());
}

} // namespace mini_hmi
