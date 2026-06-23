// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <functional>
#include <memory>
#include <tuple>

#include "ClimateHalBase.hpp"

namespace Minivi
{
    class ClimateHal : public ClimateHalBase
    {
    public:
        explicit ClimateHal();
        ~ClimateHal() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void applyState(serp::ResponsePtr<bool> reply, const ClimateZone& zone, const int32_t& temp, const int32_t& fan) override;
        void frame(serp::ResponsePtr<std::string> reply) override;
    };

} // namespace Minivi
