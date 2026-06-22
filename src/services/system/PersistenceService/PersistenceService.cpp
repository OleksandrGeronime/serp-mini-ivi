// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "PersistenceService.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <mutex>
#include <sstream>
#include <vector>

namespace Minivi
{
    std::shared_ptr<IPersistenceService> IPersistenceService::create() {
        return std::make_shared<PersistenceService>();
    }

    PersistenceService::PersistenceService()
        : PersistenceServiceBase()
    {
        logMethod("PersistenceService");
    }

    PersistenceService::~PersistenceService() noexcept = default;

    void PersistenceService::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logWarn() << "PersistenceService::onInit is not implemented";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void PersistenceService::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logWarn() << "PersistenceService::onDeinit is not implemented";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void PersistenceService::getValue(serp::ResponsePtr<std::string> reply, const std::string& key)
    {
        logWarn() << "PersistenceService::getValue is not implemented in product implementation";
        std::string result{};
        reply->call(result);
    }

    void PersistenceService::setValue(serp::ResponsePtr<bool> reply, const std::string& key, const std::string& value)
    {
        logWarn() << "PersistenceService::setValue is not implemented in product implementation";
        bool result{};
        reply->call(result);
    }

    void PersistenceService::dump(serp::ResponsePtr<std::string> reply)
    {
        logWarn() << "PersistenceService::dump is not implemented in product implementation";
        std::string result{};
        reply->call(result);
    }

} // namespace Minivi
