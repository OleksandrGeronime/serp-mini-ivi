// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <functional>
#include <memory>
#include <tuple>

#include "PhoneServiceBase.hpp"

namespace Minivi
{
    class PhoneService : public PhoneServiceBase
    {
    public:
        explicit PhoneService();
        ~PhoneService() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void incoming(serp::ResponsePtr<bool> reply, const std::string& caller) override;
        void accept(serp::ResponsePtr<bool> reply, const std::string& user) override;
        void end(serp::ResponsePtr<bool> reply) override;
        void callState(serp::ResponsePtr<std::string> reply) override;
        void callNumber(serp::ResponsePtr<std::string> reply) override;
        void frame(serp::ResponsePtr<std::string> reply) override;
    };

} // namespace Minivi
