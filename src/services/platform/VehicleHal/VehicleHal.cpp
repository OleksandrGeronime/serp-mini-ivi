// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "VehicleHal.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <mutex>
#include <sstream>
#include <vector>

namespace Minivi
{
    std::shared_ptr<IVehicleHal> IVehicleHal::create() {
        return std::make_shared<VehicleHal>();
    }

    VehicleHal::VehicleHal()
        : VehicleHalBase()
    {
        logMethod("VehicleHal");
    }

    VehicleHal::~VehicleHal() noexcept = default;

    void VehicleHal::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logWarn() << "VehicleHal::onInit is not implemented";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void VehicleHal::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logWarn() << "VehicleHal::onDeinit is not implemented";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void VehicleHal::setSignal(serp::ResponsePtr<bool> reply, const std::string& key, const std::string& value)
    {
        logWarn() << "VehicleHal::setSignal is not implemented in product implementation";
        bool result{};
        reply->call(result);
    }

    void VehicleHal::frame(serp::ResponsePtr<std::string> reply)
    {
        logWarn() << "VehicleHal::frame is not implemented in product implementation";
        std::string result{};
        reply->call(result);
    }

} // namespace Minivi
