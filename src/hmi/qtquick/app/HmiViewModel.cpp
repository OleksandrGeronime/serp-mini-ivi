#include "HmiViewModel.hpp"

#include "HmiTypes.hpp"

#include <QStringList>

namespace mini_hmi_qt {
namespace {
QString qs(const std::string& value) {
    return QString::fromStdString(value);
}

QVariantMap navItem(const char* id, const char* title, bool active) {
    QVariantMap item;
    item["id"] = id;
    item["title"] = title;
    item["active"] = active;
    return item;
}
} // namespace

HmiViewModel::HmiViewModel(QObject* parent)
    : QObject(parent) {
    mView.message = "Ready";
    rebuild();
}

HmiViewModel::~HmiViewModel() {
    if (mStarted) {
        mClient.stop();
    }
}

QString HmiViewModel::screen() const { return qs(mini_hmi::screenId(mView.screen)); }
QString HmiViewModel::seat() const { return qs(mini_hmi::toString(mView.seat)); }
QString HmiViewModel::lastAction() const { return qs(mView.lastCommand.empty() ? "<none>" : mView.lastCommand); }
QString HmiViewModel::message() const { return qs(mView.message); }
QString HmiViewModel::mediaState() const { return qs(mView.state.media.state); }
QString HmiViewModel::mediaSource() const { return qs(mView.state.media.source); }
QString HmiViewModel::mediaTrack() const { return qs(mView.state.media.track); }
QString HmiViewModel::mediaArtist() const { return qs(mView.state.media.artist); }
QString HmiViewModel::mediaTrackIndex() const { return qs(mView.state.media.trackIndex); }
QString HmiViewModel::mediaPlaylistSize() const { return qs(mView.state.media.playlistSize); }
QString HmiViewModel::audioFocus() const { return qs(mView.state.focus.owner + " / " + mView.state.focus.mode); }
QString HmiViewModel::radioStation() const { return qs(mView.state.radio.station); }
QString HmiViewModel::radioBand()    const { return qs(mView.state.radio.band.empty() ? "FM" : mView.state.radio.band); }
QString HmiViewModel::radioFreq()    const { return qs(mView.state.radio.freq.empty() ? mView.state.radio.station : mView.state.radio.freq); }
QString HmiViewModel::radioName()    const { return qs(mView.state.radio.name); }
QString HmiViewModel::radioStatus()  const { return qs(mView.state.radio.status.empty() ? "idle" : mView.state.radio.status); }
int     HmiViewModel::radioSignal()  const { return mView.state.radio.signal; }
QString HmiViewModel::navigationState() const { return qs(mView.state.nav.state); }
QString HmiViewModel::navigationDestination() const { return qs(mView.state.nav.destination); }
QString HmiViewModel::phoneState() const { return qs(mView.state.phone.state); }
QString HmiViewModel::phoneNumber() const { return qs(mView.state.phone.number); }
QString HmiViewModel::climateDriver() const { return qs(mView.state.climate.driver); }
QString HmiViewModel::climatePassenger() const { return qs(mView.state.climate.passenger); }
QString HmiViewModel::climateFan() const { return qs(mView.state.climate.fan); }
QString HmiViewModel::profile() const { return qs(mView.state.profile.active); }
QString HmiViewModel::vehicleLine() const {
    return qs(mView.state.vehicle.gear + "  " + mView.state.vehicle.speed + " mph  moving=" + mView.state.vehicle.moving);
}
QString HmiViewModel::settingsLine() const {
    return qs(mView.state.settings.theme + " / " + mView.state.settings.units + " / brightness " + mView.state.settings.brightness);
}
QString HmiViewModel::notifications() const {
    QStringList lines;
    for (const auto& notification : mView.state.notifications) lines << qs(notification);
    return lines.isEmpty() ? "No active notifications" : lines.join("\n");
}
QString HmiViewModel::navLatitude()  const { return qs(mView.state.gps.latitude); }
QString HmiViewModel::navLongitude() const { return qs(mView.state.gps.longitude); }
QString HmiViewModel::navSpeed()     const { return qs(mView.state.gps.speed); }
QString HmiViewModel::navHeading()   const { return qs(mView.state.gps.heading); }

QVariantList HmiViewModel::navItems() const {
    const auto active = mView.screen;
    return {
        navItem("home", "Home", active == mini_hmi::Screen::Home),
        navItem("media", "Media", active == mini_hmi::Screen::Media),
        navItem("radio", "Radio", active == mini_hmi::Screen::Radio),
        navItem("navigation", "Navigation", active == mini_hmi::Screen::Navigation),
        navItem("phone", "Phone", active == mini_hmi::Screen::Phone),
        navItem("climate", "Climate", active == mini_hmi::Screen::Climate),
        navItem("settings", "Settings", active == mini_hmi::Screen::Settings),
        navItem("profile", "Profile", active == mini_hmi::Screen::Profile),
        navItem("vehicle", "Vehicle", active == mini_hmi::Screen::Vehicle),
        navItem("notifications", "Notifications", active == mini_hmi::Screen::Notifications),
    };
}

QVariantList HmiViewModel::actions() const {
    QVariantList list;
    for (std::size_t index = 0; index < mView.buttons.size(); ++index) {
        const auto& button = mView.buttons[index];
        QVariantMap item;
        item["index"] = static_cast<int>(index);
        item["label"] = qs(button.label);
        item["enabled"] = mini_hmi::isButtonAllowed(button, mView.seat);
        list.push_back(item);
    }
    return list;
}

void HmiViewModel::start() {
    if (mStarted) return;
    mClient.start();
    mStarted = true;
    refresh();
    QObject::connect(&mGpsTimer, &QTimer::timeout, this, &HmiViewModel::refresh);
    mGpsTimer.start(500);
}

void HmiViewModel::refresh() {
    if (mStarted) {
        mView.state = mClient.snapshot();
    }
    rebuild();
    emit stateChanged();
}

void HmiViewModel::openScreen(const QString& id) {
    mView.screen = mini_hmi::parseScreen(id.toStdString());
    mView.message = "Opened " + mini_hmi::screenTitle(mView.screen);
    rebuild();
    emit stateChanged();
}

void HmiViewModel::toggleSeat() {
    mView.seat = mView.seat == mini_hmi::Seat::Driver ? mini_hmi::Seat::CoDriver : mini_hmi::Seat::Driver;
    rebuild();
    emit stateChanged();
}

void HmiViewModel::triggerAction(int index) {
    if (index < 0 || static_cast<std::size_t>(index) >= mView.buttons.size()) return;
    const auto& button = mView.buttons[static_cast<std::size_t>(index)];
    if (!mini_hmi::isButtonAllowed(button, mView.seat)) {
        mView.message = "Action disabled for current seat";
        emit stateChanged();
        return;
    }
    mView.lastCommand = mini_hmi::describeAction(button.action);
    mView.message = mClient.dispatch(currentUser().toStdString(), button.action);
    refresh();
}

void HmiViewModel::rebuild() {
    mView.buttons = mini_hmi::buttonsForScreen(mView.screen);
}

QString HmiViewModel::currentUser() const {
    return mView.seat == mini_hmi::Seat::Driver ? "driver" : "codriver";
}

} // namespace mini_hmi_qt
