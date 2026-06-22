// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <functional>
#include <memory>
#include <tuple>

#include "AudioFocusServiceBase.hpp"

namespace Minivi
{
    class AudioFocusService : public AudioFocusServiceBase
    {
    public:
        explicit AudioFocusService();
        ~AudioFocusService() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void requestFocus(serp::ResponsePtr<std::string> reply, const std::string& owner, const int32_t& priority, const std::string& mode) override;
        void releaseFocus(serp::ResponsePtr<bool> reply, const std::string& owner) override;
        void focusOwner(serp::ResponsePtr<std::string> reply) override;
        void focusMode(serp::ResponsePtr<std::string> reply) override;
        void frame(serp::ResponsePtr<std::string> reply) override;
    };

} // namespace Minivi
