// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "HmiGateway.hpp"

#include "IRadioService.hpp"
#include "IMediaService.hpp"
#include "IAudioFocusService.hpp"
#include "INavigationService.hpp"
#include "IPhoneService.hpp"
#include "IClimateService.hpp"
#include "IProfileService.hpp"
#include "IVehicleStateService.hpp"
#include "ISettingsService.hpp"
#include "INotificationService.hpp"
#include "IAudioHal.hpp"

#include <iomanip>
#include <map>
#include <sstream>
#include <vector>

namespace Minivi
{

namespace {

static const std::map<std::string, std::string> kStationNames = {
    {"87.5",   "Groove Salad"},
    {"98.7",   "Secret Agent"},
    {"101.1",  "Indie Pop Rocks"},
    {"103.7",  "Beat Blender"},
    {"107.9",  "Mission Control"},
    {"AM:612", "The Trip"},
};

void parseRadioStation(const std::string& station, std::string& band, std::string& freq, std::string& name)
{
    if (station.empty()) {
        band = "FM"; freq = ""; name = "";
        return;
    }
    if (station.size() > 3 && station.substr(0, 3) == "AM:") {
        band = "AM";
        freq = station.substr(3);
    } else {
        band = "FM";
        freq = station;
    }
    const auto it = kStationNames.find(station);
    name = (it != kStationNames.end()) ? it->second : station;
}

std::string extractField(const std::string& frame, const std::string& key)
{
    const std::string prefix = key + "=";
    const auto pos = frame.find(prefix);
    if (pos == std::string::npos) return {};
    const auto start = pos + prefix.size();
    const auto end = frame.find('\n', start);
    return frame.substr(start, end == std::string::npos ? std::string::npos : end - start);
}

} // namespace

    std::shared_ptr<IHmiGateway> IHmiGateway::create(std::shared_ptr<IMediaService> mediaservice, std::shared_ptr<IRadioService> radioservice, std::shared_ptr<IAudioFocusService> audiofocusservice, std::shared_ptr<INavigationService> navigationservice, std::shared_ptr<IPhoneService> phoneservice, std::shared_ptr<IClimateService> climateservice, std::shared_ptr<IProfileService> profileservice, std::shared_ptr<IVehicleStateService> vehiclestateservice, std::shared_ptr<ISettingsService> settingsservice, std::shared_ptr<INotificationService> notificationservice, std::shared_ptr<IAudioHal> audiohal) {
        return std::make_shared<HmiGateway>(mediaservice, radioservice, audiofocusservice, navigationservice, phoneservice, climateservice, profileservice, vehiclestateservice, settingsservice, notificationservice, audiohal);
    }

    HmiGateway::HmiGateway(std::shared_ptr<IMediaService> mediaservice, std::shared_ptr<IRadioService> radioservice, std::shared_ptr<IAudioFocusService> audiofocusservice, std::shared_ptr<INavigationService> navigationservice, std::shared_ptr<IPhoneService> phoneservice, std::shared_ptr<IClimateService> climateservice, std::shared_ptr<IProfileService> profileservice, std::shared_ptr<IVehicleStateService> vehiclestateservice, std::shared_ptr<ISettingsService> settingsservice, std::shared_ptr<INotificationService> notificationservice, std::shared_ptr<IAudioHal> audiohal)
        : HmiGatewayBase(mediaservice, radioservice, audiofocusservice, navigationservice, phoneservice, climateservice, profileservice, vehiclestateservice, settingsservice, notificationservice, audiohal)
    {
        logMethod("HmiGateway");
    }

    HmiGateway::~HmiGateway() noexcept = default;

    void HmiGateway::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onInit";
        // Refresh SnapshotText every 500ms so the Qt client always sees current state
        mSnapshotTimer = serp::Timer::create(std::chrono::milliseconds{500}, true,
            [this](serp::TimerPtr) { refreshSnapshot(); });
        mSnapshotTimer->start();
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void HmiGateway::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onDeinit";
        if (mSnapshotTimer) { mSnapshotTimer->stop(); mSnapshotTimer.reset(); }
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void HmiGateway::refreshSnapshot()
    {
        // Async Frame() pulls from all service processes; results cached for the next cycle.
        mRadioService->Frame()->then(IHmiGateway::THREAD, [this](const std::string& f) {
            mCachedRadioFrame = f;
        });
        mAudioFocusService->Frame()->then(IHmiGateway::THREAD, [this](const std::string& f) {
            mCachedAudioFocusFrame = f;
        });
        mMediaService->Frame()->then(IHmiGateway::THREAD, [this](const std::string& f) {
            mCachedMediaFrame = f;
        });
        mNavigationService->Frame()->then(IHmiGateway::THREAD, [this](const std::string& f) {
            mCachedNavFrame = f;
        });
        mPhoneService->Frame()->then(IHmiGateway::THREAD, [this](const std::string& f) {
            mCachedPhoneFrame = f;
        });
        mClimateService->Frame()->then(IHmiGateway::THREAD, [this](const std::string& f) {
            mCachedClimateFrame = f;
        });
        mProfileService->Frame()->then(IHmiGateway::THREAD, [this](const std::string& f) {
            mCachedProfileFrame = f;
        });
        mVehicleStateService->Frame()->then(IHmiGateway::THREAD, [this](const std::string& f) {
            mCachedVehicleFrame = f;
        });

        // Build snapshot from cached frames (one cycle behind, updated above).
        // Each cached frame may lack a trailing '\n' (gRPC strips it), so we always add one.
        auto appendFrame = [](std::ostringstream& o, const std::string& f) {
            if (f.empty()) return;
            o << f;
            if (f.back() != '\n') o << '\n';
        };

        std::ostringstream out;
        const std::string radioStation = extractField(mCachedRadioFrame, "radio.station");
        std::string radioBand, radioFreq, radioName;
        parseRadioStation(radioStation, radioBand, radioFreq, radioName);
        appendFrame(out, mCachedRadioFrame);
        out << "radio.band="   << radioBand << "\n";
        out << "radio.freq="   << radioFreq << "\n";
        out << "radio.name="   << radioName << "\n";
        out << "radio.status=" << (radioStation.empty() ? "idle" : "playing") << "\n";
        out << "radio.signal=" << (radioStation.empty() ? 0 : 85) << "\n";
        appendFrame(out, mCachedAudioFocusFrame);
        appendFrame(out, mCachedMediaFrame);
        appendFrame(out, mCachedNavFrame);
        appendFrame(out, mCachedPhoneFrame);
        appendFrame(out, mCachedClimateFrame);
        appendFrame(out, mCachedProfileFrame);
        appendFrame(out, mCachedVehicleFrame);
        SnapshotText = out.str();
    }

    void HmiGateway::snapshot(serp::ResponsePtr<std::string> reply)
    {
        reply->call(static_cast<std::string>(SnapshotText));
    }

    void HmiGateway::dispatch(serp::ResponsePtr<std::string> reply, const std::string& user, const std::string& actionId, const std::string& args)
    {
        logInfo() << "dispatch user=" << user << " action=" << actionId << " args=" << args;

        std::vector<std::string> parts;
        {
            std::istringstream ss(args);
            std::string part;
            while (std::getline(ss, part, '|')) parts.push_back(part);
        }
        auto arg = [&](size_t i, const std::string& def = {}) -> const std::string& {
            return i < parts.size() ? parts[i] : def;
        };

        // ── Radio ────────────────────────────────────────────────────────────
        if (actionId == "radio.tuner.tune") {
            const std::string band = parts.size() > 1 ? parts[0] : "FM";
            const std::string freq = parts.size() > 1 ? parts[1] : (parts.empty() ? "" : parts[0]);
            auto p = mRadioService->Tune(band, freq);
            p->subscribe([reply](int32_t, const bool& ok) { reply->call(ok ? "ok" : "error"); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }
        if (actionId == "radio.tuner.seek") {
            auto p = mRadioService->Seek(user, arg(0, "up"));
            p->subscribe([reply](int32_t, const bool& ok) { reply->call(ok ? "ok" : "error"); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }

        // ── Media ─────────────────────────────────────────────────────────────
        if (actionId == "media.play" || actionId == "media.session.play") {
            auto p = mMediaService->Play(user, arg(0));
            p->subscribe([reply](int32_t, const bool& ok) { reply->call(ok ? "ok" : "error"); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }
        if (actionId == "media.pause" || actionId == "media.session.pause") {
            auto p = mMediaService->Pause(user);
            p->subscribe([reply](int32_t, const bool& ok) { reply->call(ok ? "ok" : "error"); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }
        if (actionId == "media.next" || actionId == "media.session.skip_next") {
            auto p = mMediaService->Next(user);
            p->subscribe([reply](int32_t, const bool& ok) { reply->call(ok ? "ok" : "error"); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }
        if (actionId == "media.session.stop") {
            auto p = mMediaService->Pause(user);
            p->subscribe([reply](int32_t, const bool& ok) { reply->call(ok ? "ok" : "error"); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }

        // ── Climate ───────────────────────────────────────────────────────────
        if (actionId == "climate.temperature" || actionId == "car.hvac.set_temperature") {
            const std::string zone = arg(0, "driver");
            const std::string temp = arg(1);
            auto p = mClimateService->SetTemperature(user, zone, std::stoi(temp.empty() ? "22" : temp));
            p->subscribe([reply](int32_t, const bool& ok) { reply->call(ok ? "ok" : "error"); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }
        if (actionId == "climate.fan" || actionId == "car.hvac.set_fan") {
            auto p = mClimateService->SetFan(std::stoi(arg(0, "0")));
            p->subscribe([reply](int32_t, const bool& ok) { reply->call(ok ? "ok" : "error"); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }

        // ── Navigation ────────────────────────────────────────────────────────
        if (actionId == "navigation.route" || actionId == "navigation.guidance.start") {
            auto p = mNavigationService->StartRoute(user, arg(0));
            p->subscribe([reply](int32_t, const bool& ok) { reply->call(ok ? "ok" : "error"); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }
        if (actionId == "navigation.cancel" || actionId == "navigation.guidance.cancel") {
            auto p = mNavigationService->Cancel();
            p->subscribe([reply](int32_t, const bool& ok) { reply->call(ok ? "ok" : "error"); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }
        if (actionId == "navigation.guidance.prompt") {
            auto p = mNavigationService->NextPrompt();
            p->subscribe([reply](int32_t, const std::string& s) { reply->call(s); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }

        // ── Vehicle ───────────────────────────────────────────────────────────
        if (actionId == "vehicle.signal" || actionId == "car.property.set") {
            auto p = mVehicleStateService->SetSignal(arg(0), arg(1));
            p->subscribe([reply](int32_t, const bool& ok) { reply->call(ok ? "ok" : "error"); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }

        // ── Audio focus ───────────────────────────────────────────────────────
        if (actionId == "audio.focus.request") {
            const std::string owner = arg(0, user);
            const int32_t priority = parts.size() > 1 ? std::stoi(parts[1]) : 1;
            const std::string mode = arg(2, "music");
            auto p = mAudioFocusService->RequestFocus(owner, priority, mode);
            p->subscribe([reply](int32_t, const std::string& r) { reply->call(r); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }
        if (actionId == "audio.focus.release") {
            auto p = mAudioFocusService->ReleaseFocus(arg(0, user));
            p->subscribe([reply](int32_t, const bool& ok) { reply->call(ok ? "ok" : "error"); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }
        if (actionId == "car.audio.set_volume") {
            auto p = mAudioHal->SetVolume(arg(0, "all"), std::stoi(arg(1, "50")));
            p->subscribe([reply](int32_t, const bool& ok) { reply->call(ok ? "ok" : "error"); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }

        // ── Phone ─────────────────────────────────────────────────────────────
        if (actionId == "phone.incoming" || actionId == "telecom.call.simulate_incoming") {
            auto p = mPhoneService->Incoming(arg(0));
            p->subscribe([reply](int32_t, const bool& ok) { reply->call(ok ? "ok" : "error"); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }
        if (actionId == "phone.accept" || actionId == "telecom.call.accept") {
            auto p = mPhoneService->Accept(user);
            p->subscribe([reply](int32_t, const bool& ok) { reply->call(ok ? "ok" : "error"); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }
        if (actionId == "phone.end" || actionId == "telecom.call.end") {
            auto p = mPhoneService->End();
            p->subscribe([reply](int32_t, const bool& ok) { reply->call(ok ? "ok" : "error"); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }

        // ── Profile ───────────────────────────────────────────────────────────
        if (actionId == "profile.load" || actionId == "car.user.profile.load") {
            auto p = mProfileService->LoadProfile(user, arg(0));
            p->subscribe([reply](int32_t, const std::string& r) { reply->call(r.empty() ? "error" : r); });
            p->subscribeError([reply](int32_t, const std::string& e) { reply->call("error:" + e); });
            return;
        }

        logWarn() << "HmiGateway::dispatch unknown action: " << actionId;
        reply->call("unknown_action:" + actionId);
    }

    void HmiGateway::frame(serp::ResponsePtr<std::string> reply)
    {
        snapshot(reply);
    }

} // namespace Minivi
