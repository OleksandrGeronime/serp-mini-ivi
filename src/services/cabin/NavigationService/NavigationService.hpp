// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <functional>
#include <memory>
#include <tuple>

#include "NavigationServiceBase.hpp"
#include "ILocationHal.hpp"

namespace Minivi
{
    class NavigationService : public NavigationServiceBase
    {
    public:
        explicit NavigationService(std::shared_ptr<ILocationHal> locationhal);
        ~NavigationService() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void startRoute(serp::ResponsePtr<bool> reply, const std::string& user, const std::string& destination) override;
        void nextPrompt(serp::ResponsePtr<std::string> reply) override;
        void cancel(serp::ResponsePtr<bool> reply) override;
        void getGuidanceState(serp::ResponsePtr<GuidanceState> reply) override;
        void guidanceDestination(serp::ResponsePtr<std::string> reply) override;
        void frame(serp::ResponsePtr<std::string> reply) override;

    private:
        uint32_t mPositionSubId{0};
        serp::StreamReader<GpsPosition> mPositionStream;
    };

} // namespace Minivi
