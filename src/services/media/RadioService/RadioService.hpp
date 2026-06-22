// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <functional>
#include <memory>
#include <tuple>

#include "RadioServiceBase.hpp"

namespace Minivi
{
    class RadioService : public RadioServiceBase
    {
    public:
        explicit RadioService(std::shared_ptr<IAudioFocusService> audiofocusservice, std::shared_ptr<IRadioHal> radiohal, std::shared_ptr<IPersistenceService> persistenceservice);
        ~RadioService() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void tune(serp::ResponsePtr<bool> reply, const std::string& user, const std::string& frequency) override;
        void seek(serp::ResponsePtr<bool> reply, const std::string& user, const std::string& direction) override;
        void tunerStation(serp::ResponsePtr<std::string> reply) override;
        void frame(serp::ResponsePtr<std::string> reply) override;
    };

} // namespace Minivi
