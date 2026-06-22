// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "NavigationService.hpp"

#include "ILocationHal.hpp"

#include <iomanip>
#include <sstream>

namespace Minivi
{
    std::shared_ptr<INavigationService> INavigationService::create(std::shared_ptr<ILocationHal> locationhal) {
        return std::make_shared<NavigationService>(locationhal);
    }

    NavigationService::NavigationService(std::shared_ptr<ILocationHal> locationhal)
        : NavigationServiceBase(locationhal)
    {
        logMethod("NavigationService");
        CurrentGuidanceState.assignFromTransport(GuidanceState::inactive);
    }

    NavigationService::~NavigationService() noexcept = default;

    void NavigationService::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onInit";
        CurrentGuidanceState = static_cast<GuidanceState>(CurrentGuidanceState);

        // Subscribe via Notification (in-process / monolith mode)
        mPositionSubId = mLocationHal->PositionUpdated.connect([this](GpsPosition pos) {
            CurrentPosition = pos;
            PositionUpdated(pos);
        });

        // Subscribe via ServerStream for gRPC multiprocess mode
        mPositionStream = mLocationHal->PositionStream(0);
        mPositionStream->onData([this](const GpsPosition& pos) {
            CurrentPosition = pos;
            PositionUpdated(pos);
        });

        reply(serp::Service::Status::SUCCESSFUL);
    }

    void NavigationService::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onDeinit";
        if (mPositionSubId) {
            mLocationHal->PositionUpdated.disconnect(mPositionSubId);
            mPositionSubId = 0;
        }
        if (mPositionStream) {
            mPositionStream->cancel();
            mPositionStream.reset();
        }
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void NavigationService::startRoute(serp::ResponsePtr<bool> reply, const std::string& user, const std::string& destination)
    {
        logInfo() << "startRoute user=" << user << " destination=" << destination;
        CurrentGuidanceDestination = destination;
        CurrentGuidanceState = GuidanceState::active;
        GuidanceChanged(GuidanceState::active, destination);
        reply->call(true);
    }

    void NavigationService::nextPrompt(serp::ResponsePtr<std::string> reply)
    {
        reply->call("Continue on current route");
    }

    void NavigationService::cancel(serp::ResponsePtr<bool> reply)
    {
        logInfo() << "cancel";
        CurrentGuidanceDestination = "";
        CurrentGuidanceState = GuidanceState::inactive;
        GuidanceChanged(GuidanceState::inactive, "");
        reply->call(true);
    }

    void NavigationService::getGuidanceState(serp::ResponsePtr<GuidanceState> reply)
    {
        reply->call(static_cast<GuidanceState>(CurrentGuidanceState));
    }

    void NavigationService::guidanceDestination(serp::ResponsePtr<std::string> reply)
    {
        reply->call(static_cast<std::string>(CurrentGuidanceDestination));
    }

    void NavigationService::frame(serp::ResponsePtr<std::string> reply)
    {
        std::ostringstream out;
        std::ostringstream ss;
        ss << static_cast<GuidanceState>(CurrentGuidanceState);
        out << "navigation.state=" << ss.str() << "\n";
        out << "navigation.destination=" << static_cast<std::string>(CurrentGuidanceDestination) << "\n";
        const GpsPosition pos = static_cast<GpsPosition>(CurrentPosition);
        out << std::fixed << std::setprecision(6);
        out << "location.latitude="  << pos.latitude  << "\n";
        out << "location.longitude=" << pos.longitude << "\n";
        out << std::setprecision(1);
        out << "location.speed="   << pos.speed     << "\n";
        out << "location.heading=" << pos.heading   << "\n";
        reply->call(out.str());
    }

} // namespace Minivi
