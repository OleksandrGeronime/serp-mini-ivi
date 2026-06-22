// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <functional>
#include <memory>
#include <tuple>

#include "ClimateServiceBase.hpp"

namespace Minivi
{
    class ClimateService : public ClimateServiceBase
    {
    public:
        explicit ClimateService();
        ~ClimateService() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void setTemperature(serp::ResponsePtr<bool> reply, const std::string& user, const std::string& zone, const int32_t& temp) override;
        void setFan(serp::ResponsePtr<bool> reply, const int32_t& fan) override;
        void applyProfile(serp::ResponsePtr<bool> reply, const std::string& profile) override;
        void hvacDriverTemperature(serp::ResponsePtr<std::string> reply) override;
        void hvacPassengerTemperature(serp::ResponsePtr<std::string> reply) override;
        void hvacFan(serp::ResponsePtr<std::string> reply) override;
        void frame(serp::ResponsePtr<std::string> reply) override;
    };

} // namespace Minivi
