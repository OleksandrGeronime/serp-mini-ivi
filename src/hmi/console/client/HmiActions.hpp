#pragma once

#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace mini_hmi {

namespace HmiActionId {
inline constexpr const char* MediaSessionPlay = "media.session.play";
inline constexpr const char* MediaSessionPause = "media.session.pause";
inline constexpr const char* MediaSessionSkipNext = "media.session.skip_next";
inline constexpr const char* MediaSessionStop = "media.session.stop";
inline constexpr const char* RadioTunerTune = "radio.tuner.tune";
inline constexpr const char* RadioTunerSeek = "radio.tuner.seek";
inline constexpr const char* NavigationGuidanceStart = "navigation.guidance.start";
inline constexpr const char* NavigationGuidancePrompt = "navigation.guidance.prompt";
inline constexpr const char* NavigationGuidanceCancel = "navigation.guidance.cancel";
inline constexpr const char* TelecomCallAccept = "telecom.call.accept";
inline constexpr const char* TelecomCallEnd = "telecom.call.end";
inline constexpr const char* TelecomCallDecline = "telecom.call.decline";
inline constexpr const char* TelecomCallDial = "telecom.call.dial";
inline constexpr const char* TelecomCallMute = "telecom.call.mute";
inline constexpr const char* TelecomCallUnmute = "telecom.call.unmute";
inline constexpr const char* TelecomCallDtmf = "telecom.call.dtmf";
inline constexpr const char* TelecomContactsSetDialQuery = "telecom.contacts.set_dial_query";
inline constexpr const char* TelecomContactsSetSearchQuery = "telecom.contacts.set_search_query";
inline constexpr const char* CarHvacSetTemperature = "car.hvac.set_temperature";
inline constexpr const char* CarHvacSetFan = "car.hvac.set_fan";
inline constexpr const char* CarHvacSetAc            = "car.hvac.set_ac";
inline constexpr const char* CarHvacSetSync          = "car.hvac.set_sync";
inline constexpr const char* CarHvacSetAutoMode      = "car.hvac.set_auto";
inline constexpr const char* CarHvacSetFrontDefrost  = "car.hvac.set_front_defrost";
inline constexpr const char* CarHvacSetRearDefrost   = "car.hvac.set_rear_defrost";
inline constexpr const char* CarUserSettingsSet = "car.user.settings.set";
inline constexpr const char* CarUserProfileLoad = "car.user.profile.load";
inline constexpr const char* CarUserProfileSave = "car.user.profile.save";
inline constexpr const char* CarPropertySet = "car.property.set";
inline constexpr const char* CarAudioSetVolume = "car.audio.set_volume";
inline constexpr const char* NotificationCenterDismiss = "notification.center.dismiss";
inline constexpr const char* SnapshotRefresh = "hmi.snapshot.refresh";
inline constexpr const char* DemoScenarioRun = "demo.scenario.run";
} // namespace HmiActionId

struct HmiAction {
    std::string id;
    std::vector<std::string> args;
};

inline HmiAction action(std::string id, std::vector<std::string> args = {}) {
    return HmiAction{std::move(id), std::move(args)};
}

inline std::string describeAction(const HmiAction& action) {
    std::ostringstream out;
    out << action.id;
    for (const auto& arg : action.args) out << " " << arg;
    return out.str();
}

} // namespace mini_hmi
