#include "HmiController.hpp"

#include <exception>
#include <stdexcept>

namespace mini_hmi {

static bool startsWith(const std::string& s, const std::string& prefix) {
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

static int tailIndex(const std::string& s, const std::string& prefix) {
    try { return std::stoi(s.substr(prefix.size())); } catch (...) { return -1; }
}

static bool isPhoneScreen(Screen s) {
    return s == Screen::Phone || s == Screen::PhoneDialer
        || s == Screen::PhoneActiveCall || s == Screen::PhoneContactSearch;
}

HmiController::HmiController(IHmiClient& client)
    : mClient(client) {
    updateStatus();
}

void HmiController::updateStatus() { mState = mClient.snapshot(); }

void HmiController::checkCallAutoNav() {
    const auto& st = mState.phone.state;
    const bool callActive = (st == "dialing" || st == "ringing" || st == "active");
    if (callActive && isPhoneScreen(mScreen) && mScreen != Screen::PhoneActiveCall) {
        mScreenHistory.push_back(mScreen);
        mScreen = Screen::PhoneActiveCall;
    } else if (!callActive && mScreen == Screen::PhoneActiveCall) {
        mMessage = "Call ended.";
        if (!mScreenHistory.empty()) {
            mScreen = mScreenHistory.back();
            mScreenHistory.pop_back();
        } else {
            mScreen = Screen::Phone;
        }
    }
}

ViewModel HmiController::viewModel() {
    updateStatus();
    checkCallAutoNav();
    return ViewModel{mScreen, mSeat, mState, mLastCommand, mMessage, buttonsForScreen(mScreen)};
}

std::string HmiController::switchScreen(Screen screen, bool pushHistory) {
    if (pushHistory && mScreen != screen) mScreenHistory.push_back(mScreen);
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
        if (action.id == "screen.back") {
            mDtmfMode = false;
            if (!mScreenHistory.empty()) {
                mScreen = mScreenHistory.back();
                mScreenHistory.pop_back();
            } else {
                mScreen = Screen::Phone;
            }
            mMessage = "Back to " + screenTitle(mScreen) + ".";
            updateStatus();
            return mMessage;
        }
        if (action.id == "seat.switch" && !action.args.empty()) {
            return switchSeat(parseSeat(action.args[0], mSeat));
        }
        if (action.id == "phone.tab" && !action.args.empty()) {
            mState.phone.activeTab = action.args[0];
            mLastCommand = "phone.tab " + action.args[0];
            mMessage = "Phone tab: " + action.args[0];
            return mMessage;
        }
        if (action.id == "phone.dial_key" && !action.args.empty()) {
            if (mDtmfMode) {
                mMessage = mClient.dispatch(toString(mSeat), HmiAction{"telecom.call.dtmf", {action.args[0]}});
                updateStatus();
                return mMessage;
            }
            const std::string newQuery = mState.phone.dialQuery + action.args[0];
            mMessage = mClient.dispatch(toString(mSeat), HmiAction{"telecom.contacts.set_dial_query", {newQuery}});
            updateStatus();
            return mMessage;
        }
        if (action.id == "phone.dial_backspace") {
            std::string q = mState.phone.dialQuery;
            if (!q.empty()) q.pop_back();
            mMessage = mClient.dispatch(toString(mSeat), HmiAction{"telecom.contacts.set_dial_query", {q}});
            updateStatus();
            return mMessage;
        }
        if (action.id == "phone.dial_clear") {
            mMessage = mClient.dispatch(toString(mSeat), HmiAction{"telecom.contacts.set_dial_query", {""}});
            updateStatus();
            return mMessage;
        }
        if (action.id == "phone.search_key" && !action.args.empty()) {
            const std::string newQuery = mState.phone.contactSearchQuery + action.args[0];
            mMessage = mClient.dispatch(toString(mSeat), HmiAction{"telecom.contacts.set_search_query", {newQuery}});
            updateStatus();
            return mMessage;
        }
        if (action.id == "phone.search_backspace") {
            std::string q = mState.phone.contactSearchQuery;
            if (!q.empty()) q.pop_back();
            mMessage = mClient.dispatch(toString(mSeat), HmiAction{"telecom.contacts.set_search_query", {q}});
            updateStatus();
            return mMessage;
        }
        if (action.id == "phone.search_clear") {
            mMessage = mClient.dispatch(toString(mSeat), HmiAction{"telecom.contacts.set_search_query", {""}});
            updateStatus();
            return mMessage;
        }
        if (action.id == "phone.open_dialer_dtmf") {
            mDtmfMode = true;
            return switchScreen(Screen::PhoneDialer);
        }
        if (action.id == "phone.dial_dtmf" && !action.args.empty()) {
            mMessage = mClient.dispatch(toString(mSeat), HmiAction{"telecom.call.dtmf", {action.args[0]}});
            updateStatus();
            return mMessage;
        }
        // dial contact by index
        auto dialByNumber = [&](const std::string& number) {
            if (number.empty()) {
                mMessage = "No number.";
                return;
            }
            mMessage = mClient.dispatch(toString(mSeat), HmiAction{"telecom.call.dial", {number}});
            updateStatus();
        };
        if (startsWith(action.id, "phone.dial.fav.")) {
            const int idx = tailIndex(action.id, "phone.dial.fav.");
            if (idx >= 0 && idx < static_cast<int>(mState.phone.favoriteContacts.size()))
                dialByNumber(mState.phone.favoriteContacts[static_cast<std::size_t>(idx)].number);
            else mMessage = "Invalid contact.";
            return mMessage;
        }
        if (startsWith(action.id, "phone.dial.contact.")) {
            const int idx = tailIndex(action.id, "phone.dial.contact.");
            const auto& list = mState.phone.contactSearchQuery.empty()
                ? mState.phone.contacts : mState.phone.contactSearchResults;
            if (idx >= 0 && idx < static_cast<int>(list.size()))
                dialByNumber(list[static_cast<std::size_t>(idx)].number);
            else mMessage = "Invalid contact.";
            return mMessage;
        }
        if (startsWith(action.id, "phone.dial.recent.")) {
            const int idx = tailIndex(action.id, "phone.dial.recent.");
            if (idx >= 0 && idx < static_cast<int>(mState.phone.recentCalls.size()))
                dialByNumber(mState.phone.recentCalls[static_cast<std::size_t>(idx)].number);
            else mMessage = "Invalid contact.";
            return mMessage;
        }
        if (startsWith(action.id, "phone.dial.filtered.")) {
            const int idx = tailIndex(action.id, "phone.dial.filtered.");
            if (idx >= 0 && idx < static_cast<int>(mState.phone.dialFilteredContacts.size()))
                dialByNumber(mState.phone.dialFilteredContacts[static_cast<std::size_t>(idx)].number);
            else mMessage = "Invalid contact.";
            return mMessage;
        }
        if (startsWith(action.id, "phone.dial.search.")) {
            const int idx = tailIndex(action.id, "phone.dial.search.");
            if (idx >= 0 && idx < static_cast<int>(mState.phone.contactSearchResults.size()))
                dialByNumber(mState.phone.contactSearchResults[static_cast<std::size_t>(idx)].number);
            else mMessage = "Invalid contact.";
            return mMessage;
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
    pressButton("7");
    frames.push_back(viewModel());
    switchScreen(Screen::Media);
    pressButton("3");
    frames.push_back(viewModel());
    switchScreen(Screen::Navigation);
    pressButton("6");
    frames.push_back(viewModel());
    switchScreen(Screen::Phone);
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
