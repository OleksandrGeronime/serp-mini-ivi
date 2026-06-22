// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "AudioFocusService.hpp"

#include <sstream>

namespace Minivi
{
    std::shared_ptr<IAudioFocusService> IAudioFocusService::create() {
        return std::make_shared<AudioFocusService>();
    }

    AudioFocusService::AudioFocusService()
        : AudioFocusServiceBase()
    {
        logMethod("AudioFocusService");
    }

    AudioFocusService::~AudioFocusService() noexcept = default;

    void AudioFocusService::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onInit";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void AudioFocusService::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onDeinit";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void AudioFocusService::requestFocus(serp::ResponsePtr<std::string> reply, const std::string& owner, const int32_t& priority, const std::string& mode)
    {
        logInfo() << "requestFocus owner=" << owner << " priority=" << priority << " mode=" << mode;
        ActiveFocusOwner = owner;
        ActiveFocusMode = mode;
        FocusChanged(owner, mode, "granted");
        reply->call("granted");
    }

    void AudioFocusService::releaseFocus(serp::ResponsePtr<bool> reply, const std::string& owner)
    {
        logInfo() << "releaseFocus owner=" << owner;
        if (static_cast<std::string>(ActiveFocusOwner) == owner) {
            ActiveFocusOwner = "";
            ActiveFocusMode = "";
            FocusChanged("", "", "released");
        }
        reply->call(true);
    }

    void AudioFocusService::focusOwner(serp::ResponsePtr<std::string> reply)
    {
        reply->call(static_cast<std::string>(ActiveFocusOwner));
    }

    void AudioFocusService::focusMode(serp::ResponsePtr<std::string> reply)
    {
        reply->call(static_cast<std::string>(ActiveFocusMode));
    }

    void AudioFocusService::frame(serp::ResponsePtr<std::string> reply)
    {
        std::ostringstream out;
        out << "audio.focus.owner=" << static_cast<std::string>(ActiveFocusOwner) << "\n";
        out << "audio.focus.mode=" << static_cast<std::string>(ActiveFocusMode) << "\n";
        reply->call(out.str());
    }

} // namespace Minivi
