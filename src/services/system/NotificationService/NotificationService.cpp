// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "NotificationService.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <mutex>
#include <sstream>
#include <vector>

namespace Minivi
{
    std::shared_ptr<INotificationService> INotificationService::create(std::shared_ptr<IPersistenceService> persistenceservice) {
        return std::make_shared<NotificationService>(persistenceservice);
    }

    NotificationService::NotificationService(std::shared_ptr<IPersistenceService> persistenceservice)
        : NotificationServiceBase(persistenceservice)
    {
        logMethod("NotificationService");
    }

    NotificationService::~NotificationService() noexcept = default;

    void NotificationService::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logWarn() << "NotificationService::onInit is not implemented";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void NotificationService::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logWarn() << "NotificationService::onDeinit is not implemented";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void NotificationService::post(serp::ResponsePtr<int32_t> reply, const std::string& target, const NotificationPriority& priority, const std::string& text)
    {
        logWarn() << "NotificationService::post is not implemented in product implementation";
        int32_t result{};
        reply->call(result);
    }

    void NotificationService::listAll(serp::ResponsePtr<std::vector<Notification>> reply)
    {
        logWarn() << "NotificationService::listAll is not implemented in product implementation";
        std::vector<Notification> result{};
        reply->call(result);
    }

    void NotificationService::dismiss(serp::ResponsePtr<bool> reply, const int32_t& id)
    {
        logWarn() << "NotificationService::dismiss is not implemented in product implementation";
        bool result{};
        reply->call(result);
    }

} // namespace Minivi
