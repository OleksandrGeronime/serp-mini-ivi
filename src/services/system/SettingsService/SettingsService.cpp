// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "SettingsService.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <mutex>
#include <sstream>
#include <vector>

namespace Minivi
{
    std::shared_ptr<ISettingsService> ISettingsService::create(std::shared_ptr<IPersistenceService> persistenceservice, std::shared_ptr<INotificationService> notificationservice, std::shared_ptr<IVehicleStateService> vehiclestateservice) {
        return std::make_shared<SettingsService>(persistenceservice, notificationservice, vehiclestateservice);
    }

    SettingsService::SettingsService(std::shared_ptr<IPersistenceService> persistenceservice, std::shared_ptr<INotificationService> notificationservice, std::shared_ptr<IVehicleStateService> vehiclestateservice)
        : SettingsServiceBase(persistenceservice, notificationservice, vehiclestateservice)
    {
        logMethod("SettingsService");
    }

    SettingsService::~SettingsService() noexcept = default;

    void SettingsService::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logWarn() << "SettingsService::onInit is not implemented";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void SettingsService::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logWarn() << "SettingsService::onDeinit is not implemented";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void SettingsService::setSetting(serp::ResponsePtr<bool> reply, const std::string& user, const std::string& key, const std::string& value)
    {
        logWarn() << "SettingsService::setSetting is not implemented in product implementation";
        bool result{};
        reply->call(result);
    }

    void SettingsService::canPerform(serp::ResponsePtr<bool> reply, const std::string& user, const std::string& action)
    {
        logWarn() << "SettingsService::canPerform is not implemented in product implementation";
        bool result{};
        reply->call(result);
    }

    void SettingsService::userTheme(serp::ResponsePtr<std::string> reply)
    {
        logWarn() << "SettingsService::userTheme is not implemented in product implementation";
        std::string result{};
        reply->call(result);
    }

    void SettingsService::userUnits(serp::ResponsePtr<std::string> reply)
    {
        logWarn() << "SettingsService::userUnits is not implemented in product implementation";
        std::string result{};
        reply->call(result);
    }

    void SettingsService::displayBrightness(serp::ResponsePtr<std::string> reply)
    {
        logWarn() << "SettingsService::displayBrightness is not implemented in product implementation";
        std::string result{};
        reply->call(result);
    }

    void SettingsService::displayLayout(serp::ResponsePtr<std::string> reply)
    {
        logWarn() << "SettingsService::displayLayout is not implemented in product implementation";
        std::string result{};
        reply->call(result);
    }

    void SettingsService::frame(serp::ResponsePtr<std::string> reply)
    {
        logWarn() << "SettingsService::frame is not implemented in product implementation";
        std::string result{};
        reply->call(result);
    }

} // namespace Minivi
