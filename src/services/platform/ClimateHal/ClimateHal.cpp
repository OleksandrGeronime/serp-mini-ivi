// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "ClimateHal.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <mutex>
#include <sstream>
#include <vector>

namespace Minivi
{
    std::shared_ptr<IClimateHal> IClimateHal::create() {
        return std::make_shared<ClimateHal>();
    }

    ClimateHal::ClimateHal()
        : ClimateHalBase()
    {
        logMethod("ClimateHal");
    }

    ClimateHal::~ClimateHal() noexcept = default;

    void ClimateHal::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logWarn() << "ClimateHal::onInit is not implemented";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void ClimateHal::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logWarn() << "ClimateHal::onDeinit is not implemented";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void ClimateHal::applyState(serp::ResponsePtr<bool> reply, const std::string& zone, const int32_t& temp, const int32_t& fan)
    {
        logWarn() << "ClimateHal::applyState is not implemented in product implementation";
        bool result{};
        reply->call(result);
    }

    void ClimateHal::frame(serp::ResponsePtr<std::string> reply)
    {
        logWarn() << "ClimateHal::frame is not implemented in product implementation";
        std::string result{};
        reply->call(result);
    }

} // namespace Minivi
