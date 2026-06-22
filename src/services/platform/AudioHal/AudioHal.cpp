// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "AudioHal.hpp"

#include <sstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace Minivi
{
    std::shared_ptr<IAudioHal> IAudioHal::create() {
        return std::make_shared<AudioHal>();
    }

    AudioHal::AudioHal()
        : AudioHalBase()
    {
        logMethod("AudioHal");
        DriverVolume.assignFromTransport(50);
        PassengerVolume.assignFromTransport(50);
    }

    AudioHal::~AudioHal() noexcept = default;

    void AudioHal::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onInit";
        DriverVolume    = static_cast<int32_t>(DriverVolume);
        PassengerVolume = static_cast<int32_t>(PassengerVolume);
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void AudioHal::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onDeinit";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void AudioHal::startSource(serp::ResponsePtr<bool> reply, const std::string& source, const std::string& details)
    {
        logInfo() << "startSource source=" << source << " details=" << details;
        ActiveAudioSource  = source;
        ActiveAudioDetails = details;
        AudioRouteChanged(source, details);
        reply->call(true);
    }

    void AudioHal::stopSource(serp::ResponsePtr<bool> reply, const std::string& source)
    {
        logInfo() << "stopSource source=" << source;
        if (static_cast<std::string>(ActiveAudioSource) == source) {
            ActiveAudioSource  = "";
            ActiveAudioDetails = "";
            AudioRouteChanged("", "");
        }
        reply->call(true);
    }

    void AudioHal::playPrompt(serp::ResponsePtr<bool> reply, const std::string& prompt)
    {
        logInfo() << "playPrompt prompt=" << prompt;
        launchPrompt(prompt);
        reply->call(true);
    }

    void AudioHal::setVolume(serp::ResponsePtr<bool> reply, const std::string& zone, const int32_t& volume)
    {
        logInfo() << "setVolume zone=" << zone << " volume=" << volume;
        const int32_t clamped = volume < 0 ? 0 : (volume > 100 ? 100 : volume);
        if (zone == "driver" || zone == "all") {
            DriverVolume = clamped;
            VolumeChanged("driver", clamped);
        }
        if (zone == "passenger" || zone == "all") {
            PassengerVolume = clamped;
            VolumeChanged("passenger", clamped);
        }
        reply->call(true);
    }

    void AudioHal::frame(serp::ResponsePtr<std::string> reply)
    {
        std::ostringstream out;
        out << "audio.source="    << static_cast<std::string>(ActiveAudioSource)  << "\n";
        out << "audio.details="   << static_cast<std::string>(ActiveAudioDetails) << "\n";
        out << "audio.vol.driver="    << static_cast<int32_t>(DriverVolume)    << "\n";
        out << "audio.vol.passenger=" << static_cast<int32_t>(PassengerVolume) << "\n";
        reply->call(out.str());
    }

    void AudioHal::launchPrompt(const std::string& text)
    {
        // espeak for TTS prompts — non-fatal if not installed
        const pid_t pid = fork();
        if (pid < 0) return;
        if (pid == 0) {
            execlp("espeak", "espeak", text.c_str(), nullptr);
            _exit(0);
        }
        // detach; don't wait
        waitpid(pid, nullptr, WNOHANG);
    }

} // namespace Minivi
