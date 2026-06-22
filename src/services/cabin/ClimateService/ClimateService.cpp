// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "ClimateService.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <mutex>
#include <sstream>
#include <vector>

namespace Minivi
{
    std::shared_ptr<IClimateService> IClimateService::create() {
        return std::make_shared<ClimateService>();
    }

    ClimateService::ClimateService()
        : ClimateServiceBase()
    {
        logMethod("ClimateService");
        CurrentHvacDriverTemperature.assignFromTransport("22");
        CurrentHvacPassengerTemperature.assignFromTransport("22");
        CurrentHvacFan.assignFromTransport("2");
    }

    ClimateService::~ClimateService() noexcept = default;

    void ClimateService::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onInit";
        CurrentHvacDriverTemperature = static_cast<std::string>(CurrentHvacDriverTemperature);
        CurrentHvacPassengerTemperature = static_cast<std::string>(CurrentHvacPassengerTemperature);
        CurrentHvacFan = static_cast<std::string>(CurrentHvacFan);
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void ClimateService::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onDeinit";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void ClimateService::setTemperature(serp::ResponsePtr<bool> reply, const std::string& user, const std::string& zone, const int32_t& temp)
    {
        logInfo() << "setTemperature zone=" << zone << " temp=" << temp;
        const std::string tempStr = std::to_string(temp);
        if (zone == "driver" || zone == "all") {
            CurrentHvacDriverTemperature = tempStr;
        }
        if (zone == "passenger" || zone == "all") {
            CurrentHvacPassengerTemperature = tempStr;
        }
        reply->call(true);
    }

    void ClimateService::setFan(serp::ResponsePtr<bool> reply, const int32_t& fan)
    {
        logInfo() << "setFan level=" << fan;
        CurrentHvacFan = std::to_string(fan);
        reply->call(true);
    }

    void ClimateService::applyProfile(serp::ResponsePtr<bool> reply, const std::string& profile)
    {
        logInfo() << "applyProfile profile=" << profile;
        reply->call(true);
    }

    void ClimateService::hvacDriverTemperature(serp::ResponsePtr<std::string> reply)
    {
        reply->call(static_cast<std::string>(CurrentHvacDriverTemperature));
    }

    void ClimateService::hvacPassengerTemperature(serp::ResponsePtr<std::string> reply)
    {
        reply->call(static_cast<std::string>(CurrentHvacPassengerTemperature));
    }

    void ClimateService::hvacFan(serp::ResponsePtr<std::string> reply)
    {
        reply->call(static_cast<std::string>(CurrentHvacFan));
    }

    void ClimateService::frame(serp::ResponsePtr<std::string> reply)
    {
        std::ostringstream out;
        out << "climate.driver=" << static_cast<std::string>(CurrentHvacDriverTemperature) << "\n";
        out << "climate.passenger=" << static_cast<std::string>(CurrentHvacPassengerTemperature) << "\n";
        out << "climate.fan=" << static_cast<std::string>(CurrentHvacFan) << "\n";
        reply->call(out.str());
    }

} // namespace Minivi
