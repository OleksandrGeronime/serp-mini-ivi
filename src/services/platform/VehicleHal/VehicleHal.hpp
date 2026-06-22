// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <functional>
#include <memory>
#include <tuple>

#include "VehicleHalBase.hpp"

namespace Minivi
{
    class VehicleHal : public VehicleHalBase
    {
    public:
        explicit VehicleHal();
        ~VehicleHal() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void setSignal(serp::ResponsePtr<bool> reply, const std::string& key, const std::string& value) override;
        void frame(serp::ResponsePtr<std::string> reply) override;
    };

} // namespace Minivi
