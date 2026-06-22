// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "VehicleStateService.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <mutex>
#include <sstream>
#include <vector>

namespace Minivi
{
    std::shared_ptr<IVehicleStateService> IVehicleStateService::create(std::shared_ptr<IPersistenceService> persistenceservice, std::shared_ptr<INotificationService> notificationservice) {
        return std::make_shared<VehicleStateService>(persistenceservice, notificationservice);
    }

    VehicleStateService::VehicleStateService(std::shared_ptr<IPersistenceService> persistenceservice, std::shared_ptr<INotificationService> notificationservice)
        : VehicleStateServiceBase(persistenceservice, notificationservice)
    {
        logMethod("VehicleStateService");
        // Set default vehicle state
        CurrentIgnitionState.assignFromTransport("off");
        CurrentGearSelection.assignFromTransport("P");
        CurrentVehicleSpeed.assignFromTransport("0");
        CurrentMovingState.assignFromTransport("stationary");
    }

    VehicleStateService::~VehicleStateService() noexcept = default;

    void VehicleStateService::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onInit";
        CurrentIgnitionState = static_cast<std::string>(CurrentIgnitionState);
        CurrentGearSelection = static_cast<std::string>(CurrentGearSelection);
        CurrentVehicleSpeed = static_cast<std::string>(CurrentVehicleSpeed);
        CurrentMovingState = static_cast<std::string>(CurrentMovingState);
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void VehicleStateService::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onDeinit";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void VehicleStateService::setSignal(serp::ResponsePtr<bool> reply, const std::string& key, const std::string& value)
    {
        logInfo() << "setSignal " << key << "=" << value;
        if (key == "ignition")      { CurrentIgnitionState = value; VehiclePropertyChanged(key, value); }
        else if (key == "gear")     { CurrentGearSelection = value; VehiclePropertyChanged(key, value); }
        else if (key == "speed")    { CurrentVehicleSpeed = value;  VehiclePropertyChanged(key, value); }
        else if (key == "moving")   { CurrentMovingState = value;   VehiclePropertyChanged(key, value); }
        else { logWarn() << "VehicleStateService::setSignal unknown key: " << key; }
        reply->call(true);
    }

    void VehicleStateService::getSignal(serp::ResponsePtr<std::string> reply, const std::string& key)
    {
        if (key == "ignition")      reply->call(static_cast<std::string>(CurrentIgnitionState));
        else if (key == "gear")     reply->call(static_cast<std::string>(CurrentGearSelection));
        else if (key == "speed")    reply->call(static_cast<std::string>(CurrentVehicleSpeed));
        else if (key == "moving")   reply->call(static_cast<std::string>(CurrentMovingState));
        else                        reply->call("");
    }

    void VehicleStateService::ignitionState(serp::ResponsePtr<std::string> reply)
    {
        reply->call(static_cast<std::string>(CurrentIgnitionState));
    }

    void VehicleStateService::gearSelection(serp::ResponsePtr<std::string> reply)
    {
        reply->call(static_cast<std::string>(CurrentGearSelection));
    }

    void VehicleStateService::vehicleSpeed(serp::ResponsePtr<std::string> reply)
    {
        reply->call(static_cast<std::string>(CurrentVehicleSpeed));
    }

    void VehicleStateService::movingState(serp::ResponsePtr<std::string> reply)
    {
        reply->call(static_cast<std::string>(CurrentMovingState));
    }

    void VehicleStateService::frame(serp::ResponsePtr<std::string> reply)
    {
        std::ostringstream out;
        out << "vehicle.ignition=" << static_cast<std::string>(CurrentIgnitionState) << "\n";
        out << "vehicle.gear=" << static_cast<std::string>(CurrentGearSelection) << "\n";
        out << "vehicle.speed=" << static_cast<std::string>(CurrentVehicleSpeed) << "\n";
        out << "vehicle.moving=" << static_cast<std::string>(CurrentMovingState) << "\n";
        reply->call(out.str());
    }

} // namespace Minivi
