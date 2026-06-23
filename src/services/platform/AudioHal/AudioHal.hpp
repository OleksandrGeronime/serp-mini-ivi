// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <functional>
#include <memory>
#include <tuple>

#include "AudioHalBase.hpp"

namespace Minivi
{
    class AudioHal : public AudioHalBase
    {
    public:
        explicit AudioHal();
        ~AudioHal() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void startSource(serp::ResponsePtr<bool> reply, const std::string& source) override;
        void stopSource(serp::ResponsePtr<bool> reply, const std::string& source) override;
        void playPrompt(serp::ResponsePtr<bool> reply, const std::string& prompt) override;
        void setVolume(serp::ResponsePtr<bool> reply, const AudioZone& zone, const int32_t& volume) override;
        void frame(serp::ResponsePtr<std::string> reply) override;

    private:
        void launchPrompt(const std::string& text);
    };

} // namespace Minivi
