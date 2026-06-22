// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "PhoneHal.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <mutex>
#include <sstream>
#include <vector>

namespace Minivi
{
    std::shared_ptr<IPhoneHal> IPhoneHal::create() {
        return std::make_shared<PhoneHal>();
    }

    PhoneHal::PhoneHal()
        : PhoneHalBase()
    {
        logMethod("PhoneHal");
    }

    PhoneHal::~PhoneHal() noexcept = default;

    void PhoneHal::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onInit — loading BT contacts stub";
        // BT HFP phonebook stub — in production populated via AT+CPBR from headset
        ContactsList = "Alex:+1-555-2539,Service:+1-800-SERPCAR,Home:+1-555-0100,Work:+1-555-4400";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void PhoneHal::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logWarn() << "PhoneHal::onDeinit is not implemented";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void PhoneHal::simulateIncoming(serp::ResponsePtr<bool> reply, const std::string& caller)
    {
        logWarn() << "PhoneHal::simulateIncoming is not implemented in product implementation";
        bool result{};
        reply->call(result);
    }

    void PhoneHal::accept(serp::ResponsePtr<bool> reply)
    {
        logWarn() << "PhoneHal::accept is not implemented in product implementation";
        bool result{};
        reply->call(result);
    }

    void PhoneHal::end(serp::ResponsePtr<bool> reply)
    {
        logWarn() << "PhoneHal::end is not implemented in product implementation";
        bool result{};
        reply->call(result);
    }

    void PhoneHal::frame(serp::ResponsePtr<std::string> reply)
    {
        logWarn() << "PhoneHal::frame is not implemented in product implementation";
        std::string result{};
        reply->call(result);
    }

} // namespace Minivi
