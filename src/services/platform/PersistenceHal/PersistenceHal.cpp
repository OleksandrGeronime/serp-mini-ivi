// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "PersistenceHal.hpp"

#include <sstream>

namespace Minivi
{
    std::shared_ptr<IPersistenceHal> IPersistenceHal::create() {
        return std::make_shared<PersistenceHal>();
    }

    PersistenceHal::PersistenceHal()
        : PersistenceHalBase()
    {
        logMethod("PersistenceHal");
    }

    PersistenceHal::~PersistenceHal() noexcept = default;

    void PersistenceHal::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onInit";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void PersistenceHal::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onDeinit";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void PersistenceHal::getValue(serp::ResponsePtr<std::string> reply, const std::string& key)
    {
        const auto it = mStore.find(key);
        reply->call(it != mStore.end() ? it->second : std::string{});
    }

    void PersistenceHal::setValue(serp::ResponsePtr<bool> reply, const std::string& key, const std::string& value)
    {
        mStore[key] = value;
        reply->call(true);
    }

    void PersistenceHal::removeValue(serp::ResponsePtr<bool> reply, const std::string& key)
    {
        reply->call(mStore.erase(key) > 0);
    }

    void PersistenceHal::dump(serp::ResponsePtr<std::string> reply)
    {
        std::ostringstream out;
        for (const auto& [k, v] : mStore)
            out << k << "=" << v << "\n";
        reply->call(out.str());
    }

} // namespace Minivi
