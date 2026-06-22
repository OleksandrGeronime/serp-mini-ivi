#include "Minivi.hpp"

#include "HmiActions.hpp"

#include <algorithm>
#include <chrono>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace mini_hmi {
namespace {
using namespace std::chrono_literals;

template <typename T>
void initService(const std::shared_ptr<T>& service, const std::string& name) {
    auto base = std::dynamic_pointer_cast<serp::Service>(service);
    if (!base) throw std::runtime_error("Not a SERP service: " + name);
    auto promise = base->Init();
    if (promise) {
        promise->subscribe([name](int32_t, const serp::Service::Status& status) {
            if (status != serp::Service::Status::SUCCESSFUL) serp::logError() << "Init failed: " << name;
        });
        promise->subscribeError([name](int32_t, const std::string& error) {
            serp::logError() << "Init failed: " << name << ": " << error;
        });
    }
}

template <typename T>
void destroyService(std::shared_ptr<T>& service) {
    if (auto base = std::dynamic_pointer_cast<serp::Service>(service)) base->destroy();
    service.reset();
}

std::string argOr(const HmiAction& action, std::size_t index, const std::string& fallback = {}) {
    return index < action.args.size() ? action.args[index] : fallback;
}

int intArgOr(const HmiAction& action, std::size_t index, int fallback) {
    try {
        return std::stoi(argOr(action, index, std::to_string(fallback)));
    } catch (...) {
        return fallback;
    }
}

template<typename T>
std::string toStr(const T& v) { std::ostringstream s; s << v; return s.str(); }

std::string valueOr(std::string value, const std::string& fallback) {
    return value.empty() ? fallback : std::move(value);
}
} // namespace

void Minivi::start() {
    using namespace ::Minivi;

    mPersistenceHal = IPersistenceHal::create();
    mAudioHal       = IAudioHal::create();
    mRadioHal       = IRadioHal::create();
    mVehicleHal     = IVehicleHal::create();
    mClimateHal     = IClimateHal::create();
    mPhoneHal       = IPhoneHal::create();
    mLocationHal    = ILocationHal::create();

    mPersistence     = IPersistenceService::create();
    mCarNotification = ICarNotificationManager::create(mPersistence);
    mCarProperty     = ICarPropertyService::create(mVehicleHal, mPersistence);
    mCarPower        = ICarPowerManager::create(mCarProperty);
    mCarUser         = ICarUserManager::create(mPersistence);

    mCarClimate    = ICarClimateManager::create(mClimateHal);
    mCarNavigation = ICarNavigationManager::create(mLocationHal);
    mCarPhone      = ICarPhoneManager::create(mPhoneHal);

    mCarAudio = ICarAudioManager::create(mAudioHal);
    mCarMedia = ICarMediaManager::create(mCarAudio);
    mCarRadio = ICarRadioManager::create(mCarAudio, mRadioHal, mPersistence);

    initService(mPersistenceHal, "PersistenceHal");
    initService(mAudioHal,       "AudioHal");
    initService(mRadioHal,       "RadioHal");
    initService(mVehicleHal,     "VehicleHal");
    initService(mClimateHal,     "ClimateHal");
    initService(mPhoneHal,       "PhoneHal");
    initService(mLocationHal,    "LocationHal");

    initService(mPersistence,     "PersistenceService");
    initService(mCarNotification, "CarNotificationManager");
    initService(mCarProperty,     "CarPropertyService");
    initService(mCarPower,        "CarPowerManager");
    initService(mCarUser,         "CarUserManager");

    initService(mCarClimate,    "CarClimateManager");
    initService(mCarNavigation, "CarNavigationManager");
    initService(mCarPhone,      "CarPhoneManager");

    initService(mCarAudio, "CarAudioManager");
    initService(mCarMedia, "CarMediaManager");
    initService(mCarRadio, "CarRadioManager");

    bootstrapPersistentState();
    mCarNotification->Post("all", ::Minivi::NotificationPriority::normal, "Automotive mini platform started");
}

void Minivi::bootstrapPersistentState() {
    const std::vector<std::pair<std::string, std::string>> defaults = {
        {"driver",   "theme",      },
        {"driver",   "units"       },
        {"driver",   "brightness"  },
        {"driver",   "layout"      },
        {"codriver", "theme"       },
        {"codriver", "units"       },
        {"codriver", "brightness"  },
        {"codriver", "layout"      },
    };
    const std::vector<std::string> defValues = {
        "dark", "imperial", "80", "split",
        "light", "imperial", "75", "passenger"
    };

    for (std::size_t i = 0; i < defaults.size(); ++i) {
        const auto& [user, key] = defaults[i];
        const auto& fallback = defValues[i];
        const std::string storeKey = "settings." + user + "." + key;
        auto promise = mPersistence->GetValue(storeKey);
        if (promise) {
            promise->subscribe([carUser = mCarUser, user, key, fallback](int32_t, const std::string& stored) {
                carUser->SetSetting(user, key, stored.empty() ? fallback : stored);
            });
            promise->subscribeError([carUser = mCarUser, user, key, fallback](int32_t, const std::string&) {
                carUser->SetSetting(user, key, fallback);
            });
        } else {
            mCarUser->SetSetting(user, key, fallback);
        }
    }
}

void Minivi::stop() {
    destroyService(mCarRadio);
    destroyService(mCarMedia);
    destroyService(mCarAudio);
    destroyService(mCarPhone);
    destroyService(mCarNavigation);
    destroyService(mCarClimate);
    destroyService(mCarUser);
    destroyService(mCarPower);
    destroyService(mCarProperty);
    destroyService(mCarNotification);
    destroyService(mPersistence);
    destroyService(mLocationHal);
    destroyService(mPhoneHal);
    destroyService(mClimateHal);
    destroyService(mVehicleHal);
    destroyService(mRadioHal);
    destroyService(mAudioHal);
    destroyService(mPersistenceHal);
    serp::App::stopAll();
}

ScreenData Minivi::snapshot() {
    ScreenData data;

    auto str = [](auto& prop) { return static_cast<std::string>(prop); };

    data.vehicle.ignition = valueOr(str(mCarProperty->CurrentIgnitionState), data.vehicle.ignition);
    data.vehicle.gear     = valueOr(str(mCarProperty->CurrentGearSelection),  data.vehicle.gear);
    data.vehicle.speed    = valueOr(str(mCarProperty->CurrentVehicleSpeed),   data.vehicle.speed);
    data.vehicle.moving   = valueOr(str(mCarProperty->CurrentMovingState),    data.vehicle.moving);

    data.climate.driver    = valueOr(str(mCarClimate->CurrentHvacDriverTemperature),    data.climate.driver);
    data.climate.passenger = valueOr(str(mCarClimate->CurrentHvacPassengerTemperature), data.climate.passenger);
    data.climate.fan       = valueOr(str(mCarClimate->CurrentHvacFan),                  data.climate.fan);

    data.nav.state       = valueOr(toStr(static_cast<::Minivi::GuidanceState>(mCarNavigation->CurrentGuidanceState)), data.nav.state);
    data.nav.destination = valueOr(str(mCarNavigation->CurrentGuidanceDestination), data.nav.destination);

    data.phone.state  = valueOr(str(mCarPhone->CurrentCallState),  data.phone.state);
    data.phone.number = valueOr(str(mCarPhone->CurrentCallNumber), data.phone.number);

    data.profile.active      = valueOr(str(mCarUser->CurrentActiveProfile),     data.profile.active);
    data.settings.theme      = valueOr(str(mCarUser->CurrentUserTheme),          data.settings.theme);
    data.settings.units      = valueOr(str(mCarUser->CurrentUserUnits),          data.settings.units);
    data.settings.brightness = valueOr(str(mCarUser->CurrentDisplayBrightness), data.settings.brightness);
    data.settings.layout     = valueOr(str(mCarUser->CurrentDisplayLayout),      data.settings.layout);

    data.focus.owner = valueOr(str(mCarAudio->ActiveFocusOwner), data.focus.owner);
    data.focus.mode  = valueOr(str(mCarAudio->ActiveFocusMode),  data.focus.mode);

    data.media.state  = valueOr(toStr(static_cast<::Minivi::PlaybackState>(mCarMedia->CurrentPlaybackState)), data.media.state);
    data.media.source = valueOr(str(mCarMedia->CurrentPlaybackSource), data.media.source);
    const auto track  = static_cast<std::optional<::Minivi::TrackInfo>>(mCarMedia->CurrentPlaybackTrack);
    if (track) {
        data.media.track  = track->title;
        data.media.artist = track->artist;
    }

    data.radio.station = valueOr(str(mCarRadio->CurrentTunerStation), data.radio.station);
    data.radio.freq    = data.radio.station;

    return data;
}

std::string Minivi::dispatch(const std::string& user, const HmiAction& action) {
    namespace action_id = HmiActionId;

    if (action.id == action_id::MediaSessionPlay) {
        mCarMedia->Play(user, argOr(action, 0, "demo_playlist"));
    } else if (action.id == action_id::MediaSessionPause) {
        mCarMedia->Pause("hmi");
    } else if (action.id == action_id::MediaSessionSkipNext) {
        mCarMedia->Next(user);
    } else if (action.id == action_id::MediaSessionStop) {
        mCarMedia->Pause("stop");
    } else if (action.id == action_id::RadioTunerTune) {
        mCarRadio->Tune(user, argOr(action, 0, "101.1"));
    } else if (action.id == action_id::RadioTunerSeek) {
        mCarRadio->Seek(user, argOr(action, 0, "up"));
    } else if (action.id == action_id::NavigationGuidanceStart) {
        mCarNavigation->StartRoute(user, argOr(action, 0, "Home"));
    } else if (action.id == action_id::NavigationGuidancePrompt) {
        mCarNavigation->NextPrompt();
    } else if (action.id == action_id::NavigationGuidanceCancel) {
        mCarNavigation->Cancel();
    } else if (action.id == action_id::TelecomCallSimulateIncoming) {
        mCarPhone->Incoming(argOr(action, 0, "+1-555-0101"));
    } else if (action.id == action_id::TelecomCallAccept) {
        mCarPhone->Accept(user);
    } else if (action.id == action_id::TelecomCallEnd) {
        mCarPhone->End();
    } else if (action.id == action_id::CarHvacSetTemperature) {
        mCarClimate->SetTemperature(user, argOr(action, 0, "driver"), intArgOr(action, 1, 22));
    } else if (action.id == action_id::CarHvacSetFan) {
        mCarClimate->SetFan(intArgOr(action, 0, 2));
    } else if (action.id == action_id::CarUserSettingsSet) {
        mCarUser->SetSetting(user, argOr(action, 0, "theme"), argOr(action, 1, "dark"));
    } else if (action.id == action_id::CarUserProfileLoad) {
        mCarUser->LoadProfile(user, argOr(action, 0, "default"));
    } else if (action.id == action_id::CarUserProfileSave) {
        mCarUser->SaveProfile(user, argOr(action, 0, "default"));
    } else if (action.id == action_id::CarPropertySet) {
        mCarProperty->SetProperty(argOr(action, 0, "speed"), argOr(action, 1, "0"));
    } else if (action.id == action_id::CarAudioSetVolume) {
        mAudioHal->SetVolume(argOr(action, 0, "cabin"), intArgOr(action, 1, 50));
    } else if (action.id == action_id::NotificationCenterDismiss) {
        mCarNotification->Dismiss(intArgOr(action, 0, 1));
    } else if (action.id == action_id::SnapshotRefresh) {
        (void)snapshot();
        return "ok";
    }
    return "queued";
}

} // namespace mini_hmi
