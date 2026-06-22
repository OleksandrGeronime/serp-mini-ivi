// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <functional>
#include <memory>
#include <tuple>

#include "NotificationServiceBase.hpp"

namespace Minivi
{
    class NotificationService : public NotificationServiceBase
    {
    public:
        explicit NotificationService(std::shared_ptr<IPersistenceService> persistenceservice);
        ~NotificationService() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void post(serp::ResponsePtr<int32_t> reply, const std::string& target, const NotificationPriority& priority, const std::string& text) override;
        void listAll(serp::ResponsePtr<std::vector<Notification>> reply) override;
        void dismiss(serp::ResponsePtr<bool> reply, const int32_t& id) override;
    };

} // namespace Minivi
