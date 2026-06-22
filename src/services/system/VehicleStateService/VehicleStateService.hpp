// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <functional>
#include <memory>
#include <tuple>

#include "VehicleStateServiceBase.hpp"

namespace Minivi
{
    class VehicleStateService : public VehicleStateServiceBase
    {
    public:
        explicit VehicleStateService(std::shared_ptr<IPersistenceService> persistenceservice, std::shared_ptr<INotificationService> notificationservice);
        ~VehicleStateService() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void setSignal(serp::ResponsePtr<bool> reply, const std::string& key, const std::string& value) override;
        void getSignal(serp::ResponsePtr<std::string> reply, const std::string& key) override;
        void ignitionState(serp::ResponsePtr<std::string> reply) override;
        void gearSelection(serp::ResponsePtr<std::string> reply) override;
        void vehicleSpeed(serp::ResponsePtr<std::string> reply) override;
        void movingState(serp::ResponsePtr<std::string> reply) override;
        void frame(serp::ResponsePtr<std::string> reply) override;
    };

} // namespace Minivi
