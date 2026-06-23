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
        PhoneState.assignFromTransport(Minivi::PhoneCallState::idle);
        PhoneCaller.assignFromTransport("");
    }

    PhoneHal::~PhoneHal() noexcept = default;

    void PhoneHal::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onInit — loading BT contacts stub";
        // BT HFP phonebook stub — in production populated via AT+CPBR from headset
        ContactsList = std::vector<Contact>{
            {"Alex",    "+1-555-2539"},
            {"Service", "+1-800-SERPCAR"},
            {"Home",    "+1-555-0100"},
            {"Work",    "+1-555-4400"},
        };
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void PhoneHal::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logWarn() << "PhoneHal::onDeinit is not implemented";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void PhoneHal::simulateIncoming(serp::ResponsePtr<bool> reply, const std::string& caller)
    {
        PhoneState = Minivi::PhoneCallState::ringing;
        PhoneCaller = caller;
        PhoneStateChanged(Minivi::PhoneCallState::ringing, caller);
        reply->call(true);
    }

    void PhoneHal::accept(serp::ResponsePtr<bool> reply)
    {
        PhoneState = Minivi::PhoneCallState::active;
        PhoneStateChanged(Minivi::PhoneCallState::active, static_cast<std::string>(PhoneCaller));
        reply->call(true);
    }

    void PhoneHal::end(serp::ResponsePtr<bool> reply)
    {
        PhoneState = Minivi::PhoneCallState::idle;
        PhoneCaller = "";
        PhoneStateChanged(Minivi::PhoneCallState::idle, "");
        reply->call(true);
    }

    void PhoneHal::frame(serp::ResponsePtr<std::string> reply)
    {
        std::ostringstream out;
        out << "phonehal.state="    << static_cast<Minivi::PhoneCallState>(PhoneState)                  << "\n";
        out << "phonehal.caller="   << static_cast<std::string>(PhoneCaller)                         << "\n";
        out << "phonehal.contacts=" << static_cast<std::vector<Contact>>(ContactsList).size()        << "\n";
        reply->call(out.str());
    }

} // namespace Minivi
