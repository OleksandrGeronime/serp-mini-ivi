#include "RemoteHmiClient.hpp"

#include "HmiActions.hpp"

#include <algorithm>
#include <sstream>
#include <stdexcept>

// ── transport proxy declarations ─────────────────────────────────────────────
#if defined(MINI_HMI_REMOTE_GRPC)
#include "grpc/CarPropertyServiceProxy.hpp"
#include "grpc/CarNotificationManagerProxy.hpp"
#include "grpc/CarClimateManagerProxy.hpp"
#include "grpc/CarNavigationManagerProxy.hpp"
#include "grpc/CarPhoneManagerProxy.hpp"
#include "grpc/CarUserManagerProxy.hpp"
#include "grpc/CarMediaManagerProxy.hpp"
#include "grpc/CarRadioManagerProxy.hpp"
#else
#include "dbus/CarPropertyServiceProxy.hpp"
#include "dbus/CarNotificationManagerProxy.hpp"
#include "dbus/CarClimateManagerProxy.hpp"
#include "dbus/CarNavigationManagerProxy.hpp"
#include "dbus/CarPhoneManagerProxy.hpp"
#include "dbus/CarUserManagerProxy.hpp"
#include "dbus/CarMediaManagerProxy.hpp"
#include "dbus/CarRadioManagerProxy.hpp"
#endif

namespace mini_hmi {
namespace {

std::string argOr(const HmiAction& action, std::size_t index, const std::string& fallback = {}) {
    return index < action.args.size() ? action.args[index] : fallback;
}

int intArgOr(const HmiAction& action, std::size_t index, int fallback) {
    try {
        const auto s = argOr(action, index, std::to_string(fallback));
        return std::stoi(s);
    } catch (...) {
        return fallback;
    }
}

double doubleArgOr(const HmiAction& action, std::size_t index, double fallback) {
    try {
        const auto s = argOr(action, index, std::to_string(fallback));
        return std::stod(s);
    } catch (...) {
        return fallback;
    }
}

} // namespace

void RemoteHmiClient::start() {
    serp::transport::Registry::clear();

#if defined(MINI_HMI_REMOTE_GRPC)
    namespace transport = Minivi::serp_transport::grpc;
    mBackend = serp::transport::createBackend("grpc");
#else
    namespace transport = Minivi::serp_transport::dbus;
    mBackend = serp::transport::createBackend("dbus");
#endif

    mCarProperty     = transport::create_carpropertyservice_icarpropertyservice_proxy("hmi");
    mCarNotification = transport::create_carnotificationmanager_icarnotificationmanager_proxy("hmi");
    mCarClimate      = transport::create_carclimatemanager_icarclimatemanager_proxy("hmi");
    mCarNavigation   = transport::create_carnavigationmanager_icarnavigationmanager_proxy("hmi");
    mCarPhone        = transport::create_carphonemanager_icarphonemanager_proxy("hmi");
    mCarUser         = transport::create_carusermanager_icarusermanager_proxy("hmi");
    mCarMedia        = transport::create_carmediamanager_icarmediamanager_proxy("hmi");
    mCarRadio        = transport::create_carradiomanager_icarradiomanager_proxy("hmi");

    mBackend->start();

    // Subscribe to notification events to maintain a local list
    mNotifPostedSubId = mCarNotification->NotificationPosted.connect(
        [this](const Minivi::CarNotification& n) {
            std::lock_guard<std::mutex> lock(mNotifMutex);
            mNotifCache.push_back({n.id, n.title});
        });
    mNotifDismissedSubId = mCarNotification->NotificationDismissed.connect(
        [this](int32_t id) {
            std::lock_guard<std::mutex> lock(mNotifMutex);
            mNotifCache.erase(
                std::remove_if(mNotifCache.begin(), mNotifCache.end(),
                               [id](const NotifEntry& e) { return e.id == id; }),
                mNotifCache.end());
        });

    mSettingChangedSubId = mCarUser->SettingChanged.connect(
        [this](const std::string& key, const std::string& value) {
            std::lock_guard<std::mutex> lock(mSettingsMutex);
            mSettingsCache[key] = value;
        });
}

void RemoteHmiClient::stop() {
    if (mCarNotification) {
        if (mNotifPostedSubId)    mCarNotification->NotificationPosted.disconnect(mNotifPostedSubId);
        if (mNotifDismissedSubId) mCarNotification->NotificationDismissed.disconnect(mNotifDismissedSubId);
        mNotifPostedSubId = 0;
        mNotifDismissedSubId = 0;
    }
    if (mCarUser && mSettingChangedSubId) {
        mCarUser->SettingChanged.disconnect(mSettingChangedSubId);
        mSettingChangedSubId = 0;
    }
    mCarProperty.reset();
    mCarNotification.reset();
    mCarClimate.reset();
    mCarNavigation.reset();
    mCarPhone.reset();
    mCarUser.reset();
    mCarMedia.reset();
    mCarRadio.reset();
    if (mBackend) {
        mBackend->stop();
        mBackend.reset();
    }
}

ScreenData RemoteHmiClient::snapshot() {
    if (!mCarProperty) throw std::runtime_error("RemoteHmiClient is not started");

    mLastSnapshot.vehicle.ignition = static_cast<bool>(mCarProperty->Ignition) ? "true" : "false";
    mLastSnapshot.vehicle.gear     = static_cast<std::string>(mCarProperty->Gear);
    mLastSnapshot.vehicle.speed    = toStr(static_cast<double>(mCarProperty->Speed));
    mLastSnapshot.vehicle.moving   = static_cast<bool>(mCarProperty->Moving) ? "true" : "false";

    mLastSnapshot.climate.driver    = toStr(static_cast<double>(mCarClimate->DriverTemp));
    mLastSnapshot.climate.passenger = toStr(static_cast<double>(mCarClimate->PassengerTemp));
    mLastSnapshot.climate.fan       = toStr(static_cast<int32_t>(mCarClimate->FanSpeed));
    mLastSnapshot.climate.ac        = static_cast<bool>(mCarClimate->AcOn) ? "on" : "off";
    mLastSnapshot.climate.sync      = static_cast<bool>(mCarClimate->SyncOn) ? "on" : "off";
    mLastSnapshot.climate.autoMode  = static_cast<bool>(mCarClimate->AutoOn) ? "on" : "off";
    mLastSnapshot.climate.frontDefrost = static_cast<bool>(mCarClimate->FrontDefrost) ? "on" : "off";
    mLastSnapshot.climate.rearDefrost  = static_cast<bool>(mCarClimate->RearDefrost) ? "on" : "off";

    mLastSnapshot.nav.state       = toStr(static_cast<Minivi::GuidanceState>(mCarNavigation->CurrentGuidanceState));
    mLastSnapshot.nav.destination = static_cast<std::string>(mCarNavigation->Destination);

    mLastSnapshot.gps.latitude  = toStr(static_cast<double>(mCarNavigation->Latitude));
    mLastSnapshot.gps.longitude = toStr(static_cast<double>(mCarNavigation->Longitude));
    mLastSnapshot.gps.speed     = toStr(static_cast<double>(mCarNavigation->Speed));
    mLastSnapshot.gps.heading   = toStr(static_cast<double>(mCarNavigation->Heading));

    mLastSnapshot.phone.state  = toStr(static_cast<int32_t>(mCarPhone->CurrentCallState));
    mLastSnapshot.phone.number = static_cast<std::string>(mCarPhone->PhoneNumber);

    mLastSnapshot.profile.active      = static_cast<std::string>(mCarUser->CurrentUser);
    mLastSnapshot.settings.theme      = static_cast<std::string>(mCarUser->Theme);
    mLastSnapshot.settings.units      = static_cast<std::string>(mCarUser->Units);
    mLastSnapshot.settings.brightness = toStr(static_cast<int32_t>(mCarUser->Brightness));
    mLastSnapshot.settings.layout     = "";

    mLastSnapshot.focus.owner = "n/a";
    mLastSnapshot.focus.mode  = "n/a";

    mLastSnapshot.media.state  = toStr(static_cast<Minivi::PlaybackState>(mCarMedia->CurrentPlaybackState));
    mLastSnapshot.media.source = static_cast<std::string>(mCarMedia->CurrentPlaybackSource);
    const auto track = static_cast<std::optional<Minivi::TrackInfo>>(mCarMedia->CurrentTrack);
    if (track) {
        mLastSnapshot.media.track  = track->title;
        mLastSnapshot.media.artist = track->artist;
    }
    mLastSnapshot.media.trackIndex    = toStr(static_cast<int32_t>(mCarMedia->TrackIndex));
    mLastSnapshot.media.playlistSize  = toStr(static_cast<uint32_t>(mCarMedia->PlaylistSize));

    // Radio — signal > 0 means stream active
    const std::string station = static_cast<std::string>(mCarRadio->Station);
    const int signal = static_cast<int32_t>(mCarRadio->Signal);
    mLastSnapshot.radio.station = station;
    mLastSnapshot.radio.name    = static_cast<std::string>(mCarRadio->Name);
    mLastSnapshot.radio.signal  = signal;
    if (!station.empty()) {
        mLastSnapshot.radio.freq = (station.size() > 3 && station.substr(0, 3) == "AM:")
                                     ? station.substr(3) : station;
        mLastSnapshot.radio.band = (station.substr(0, 3) == "AM:") ? "AM" : "FM";
    } else {
        mLastSnapshot.radio.freq = "";
        mLastSnapshot.radio.band = "FM";
    }
    mLastSnapshot.radio.status = (signal > 0) ? "playing" : "idle";

    // Phone contacts from CarPhoneManager
    const auto contacts = static_cast<std::vector<std::string>>(mCarPhone->Contacts);
    {
        std::ostringstream oss;
        for (std::size_t i = 0; i < contacts.size(); ++i) {
            if (i > 0) oss << ",";
            oss << contacts[i];
        }
        mLastSnapshot.phone.contacts = oss.str();
    }

    // Notifications — copy from subscription cache
    {
        std::lock_guard<std::mutex> lock(mNotifMutex);
        mLastSnapshot.notifications.clear();
        for (const auto& e : mNotifCache)
            mLastSnapshot.notifications.push_back(e.text);
    }

    return mLastSnapshot;
}

std::string RemoteHmiClient::dispatch(const std::string& user, const HmiAction& action) {
    if (!mCarProperty) throw std::runtime_error("RemoteHmiClient is not started");

    namespace action_id = HmiActionId;

    if (action.id == action_id::RadioTunerTune || action.id == "radio.tuner.tune") {
        const std::string freq = argOr(action, 1).empty() ? argOr(action, 0, "87.5") : argOr(action, 1);
        mCarRadio->Tune(freq);

    } else if (action.id == action_id::RadioTunerSeek || action.id == "radio.tuner.seek") {
        mCarRadio->Seek(argOr(action, 0, "up"));

    } else if (action.id == "media.play.at") {
        mCarMedia->PlayAt(intArgOr(action, 0, 0));

    } else if (action.id == action_id::MediaSessionPlay || action.id == "media.play" || action.id == "media.session.play") {
        mCarMedia->Play(argOr(action, 0));

    } else if (action.id == action_id::MediaSessionPause || action.id == "media.pause" || action.id == "media.session.pause" || action.id == "media.session.stop") {
        mCarMedia->Pause();

    } else if (action.id == action_id::MediaSessionSkipNext || action.id == "media.next" || action.id == "media.session.skip_next") {
        mCarMedia->Next();

    } else if (action.id == "media.previous" || action.id == "media.session.skip_prev") {
        mCarMedia->Previous();

    } else if (action.id == action_id::CarHvacSetTemperature || action.id == "climate.temperature" || action.id == "car.hvac.set_temperature") {
        mCarClimate->SetTemperature(argOr(action, 0, "driver"), doubleArgOr(action, 1, 22.0));

    } else if (action.id == action_id::CarHvacSetFan || action.id == "climate.fan" || action.id == "car.hvac.set_fan") {
        mCarClimate->SetFanSpeed(intArgOr(action, 0, 2));

    } else if (action.id == action_id::NavigationGuidanceStart || action.id == "navigation.route" || action.id == "navigation.guidance.start") {
        mCarNavigation->StartRoute(argOr(action, 0));

    } else if (action.id == action_id::NavigationGuidanceCancel || action.id == "navigation.cancel" || action.id == "navigation.guidance.cancel") {
        mCarNavigation->CancelRoute();

    } else if (action.id == action_id::NavigationGuidancePrompt || action.id == "navigation.guidance.prompt") {
        mCarNavigation->NextPrompt();

    } else if (action.id == action_id::TelecomCallSimulateIncoming || action.id == "phone.incoming" || action.id == "telecom.call.simulate_incoming") {
        mCarPhone->Incoming(argOr(action, 0, "+1-555-0101"));

    } else if (action.id == action_id::TelecomCallAccept || action.id == "phone.accept" || action.id == "telecom.call.accept") {
        mCarPhone->AcceptCall();

    } else if (action.id == action_id::TelecomCallEnd || action.id == "phone.end" || action.id == "telecom.call.end") {
        mCarPhone->EndCall();

    } else if (action.id == "phone.dial") {
        mCarPhone->Dial(argOr(action, 0));

    } else if (action.id == action_id::CarUserProfileLoad || action.id == "profile.load" || action.id == "car.user.profile.load") {
        mCarUser->LoadProfile(argOr(action, 0, user));

    } else if (action.id == action_id::CarUserProfileSave) {
        // SaveProfile removed from interface — no-op

    } else if (action.id == action_id::CarUserSettingsSet || action.id == "car.user.settings.set") {
        const std::string key = argOr(action, 0);
        const std::string val = argOr(action, 1);
        if (key == "theme")       mCarUser->SetTheme(val);
        else if (key == "units")  mCarUser->SetUnits(val);
        else if (key == "language") mCarUser->SetLanguage(val);
        else if (key == "brightness") {
            try { mCarUser->SetBrightness(std::stoi(val)); } catch (...) {}
        }

    } else if (action.id == action_id::CarPropertySet || action.id == "vehicle.signal" || action.id == "car.property.set") {
        mCarProperty->SetProperty(argOr(action, 0), argOr(action, 1));

    } else if (action.id == "audio.focus.request" || action.id == "audio.focus.release") {
        return "not available";

    } else if (action.id == action_id::NotificationCenterDismiss) {
        mCarNotification->Dismiss(intArgOr(action, 0, 1));

    } else if (action.id == action_id::CarHvacSetAc) {
        mCarClimate->ToggleAc();

    } else if (action.id == action_id::CarHvacSetSync) {
        mCarClimate->ToggleSync();

    } else if (action.id == action_id::CarHvacSetAutoMode) {
        mCarClimate->ToggleAuto();

    } else if (action.id == action_id::CarHvacSetFrontDefrost) {
        mCarClimate->ToggleFrontDefrost();

    } else if (action.id == action_id::CarHvacSetRearDefrost) {
        mCarClimate->ToggleRearDefrost();

    } else if (action.id == action_id::CarAudioSetVolume) {
        // Volume control is a HAL concern — not exposed via Car* service proxies in remote mode
        return "volume not available";
    }

    return "queued";
}

} // namespace mini_hmi
