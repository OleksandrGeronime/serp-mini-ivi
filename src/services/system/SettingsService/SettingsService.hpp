// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <functional>
#include <memory>
#include <tuple>

#include "SettingsServiceBase.hpp"

namespace Minivi
{
    class SettingsService : public SettingsServiceBase
    {
    public:
        explicit SettingsService(std::shared_ptr<IPersistenceService> persistenceservice, std::shared_ptr<INotificationService> notificationservice, std::shared_ptr<IVehicleStateService> vehiclestateservice);
        ~SettingsService() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void setSetting(serp::ResponsePtr<bool> reply, const std::string& user, const std::string& key, const std::string& value) override;
        void canPerform(serp::ResponsePtr<bool> reply, const std::string& user, const std::string& action) override;
        void userTheme(serp::ResponsePtr<std::string> reply) override;
        void userUnits(serp::ResponsePtr<std::string> reply) override;
        void displayBrightness(serp::ResponsePtr<std::string> reply) override;
        void displayLayout(serp::ResponsePtr<std::string> reply) override;
        void frame(serp::ResponsePtr<std::string> reply) override;
    };

} // namespace Minivi
