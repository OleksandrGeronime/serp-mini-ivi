// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <functional>
#include <memory>
#include <tuple>

#include "ProfileServiceBase.hpp"

namespace Minivi
{
    class ProfileService : public ProfileServiceBase
    {
    public:
        explicit ProfileService(std::shared_ptr<IClimateService> climateservice);
        ~ProfileService() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void loadProfile(serp::ResponsePtr<std::string> reply, const std::string& user, const std::string& profile) override;
        void saveProfile(serp::ResponsePtr<bool> reply, const std::string& user, const std::string& profile) override;
        void activeProfile(serp::ResponsePtr<std::string> reply) override;
        void frame(serp::ResponsePtr<std::string> reply) override;
    };

} // namespace Minivi
