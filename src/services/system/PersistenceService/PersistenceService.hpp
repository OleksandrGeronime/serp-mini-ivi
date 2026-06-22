// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <functional>
#include <memory>
#include <tuple>

#include "PersistenceServiceBase.hpp"

namespace Minivi
{
    class PersistenceService : public PersistenceServiceBase
    {
    public:
        explicit PersistenceService();
        ~PersistenceService() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void getValue(serp::ResponsePtr<std::string> reply, const std::string& key) override;
        void setValue(serp::ResponsePtr<bool> reply, const std::string& key, const std::string& value) override;
        void dump(serp::ResponsePtr<std::string> reply) override;
    };

} // namespace Minivi
