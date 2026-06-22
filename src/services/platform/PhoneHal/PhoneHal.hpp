// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <functional>
#include <memory>
#include <tuple>

#include "PhoneHalBase.hpp"

namespace Minivi
{
    class PhoneHal : public PhoneHalBase
    {
    public:
        explicit PhoneHal();
        ~PhoneHal() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void simulateIncoming(serp::ResponsePtr<bool> reply, const std::string& caller) override;
        void accept(serp::ResponsePtr<bool> reply) override;
        void end(serp::ResponsePtr<bool> reply) override;
        void frame(serp::ResponsePtr<std::string> reply) override;
    };

} // namespace Minivi
