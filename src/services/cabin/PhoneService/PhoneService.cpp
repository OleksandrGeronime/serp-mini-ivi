// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "PhoneService.hpp"

#include <sstream>

namespace Minivi
{
    std::shared_ptr<IPhoneService> IPhoneService::create() {
        return std::make_shared<PhoneService>();
    }

    PhoneService::PhoneService()
        : PhoneServiceBase()
    {
        logMethod("PhoneService");
    }

    PhoneService::~PhoneService() noexcept = default;

    void PhoneService::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onInit";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void PhoneService::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onDeinit";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void PhoneService::incoming(serp::ResponsePtr<bool> reply, const std::string& caller)
    {
        logInfo() << "incoming caller=" << caller;
        CurrentCallState = "ringing";
        CurrentCallNumber = caller;
        CallChanged("ringing", caller);
        reply->call(true);
    }

    void PhoneService::accept(serp::ResponsePtr<bool> reply, const std::string& user)
    {
        logInfo() << "accept user=" << user;
        CurrentCallState = "active";
        CallChanged("active", static_cast<std::string>(CurrentCallNumber));
        reply->call(true);
    }

    void PhoneService::end(serp::ResponsePtr<bool> reply)
    {
        logInfo() << "end";
        CurrentCallState = "";
        CurrentCallNumber = "";
        CallChanged("", "");
        reply->call(true);
    }

    void PhoneService::callState(serp::ResponsePtr<std::string> reply)
    {
        reply->call(static_cast<std::string>(CurrentCallState));
    }

    void PhoneService::callNumber(serp::ResponsePtr<std::string> reply)
    {
        reply->call(static_cast<std::string>(CurrentCallNumber));
    }

    void PhoneService::frame(serp::ResponsePtr<std::string> reply)
    {
        std::ostringstream out;
        out << "phone.state=" << static_cast<std::string>(CurrentCallState) << "\n";
        out << "phone.number=" << static_cast<std::string>(CurrentCallNumber) << "\n";
        reply->call(out.str());
    }

} // namespace Minivi
