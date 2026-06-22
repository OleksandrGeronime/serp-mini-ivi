// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#include "ProfileService.hpp"

#include <sstream>

namespace Minivi
{
    std::shared_ptr<IProfileService> IProfileService::create(std::shared_ptr<IClimateService> climateservice) {
        return std::make_shared<ProfileService>(climateservice);
    }

    ProfileService::ProfileService(std::shared_ptr<IClimateService> climateservice)
        : ProfileServiceBase(climateservice)
    {
        logMethod("ProfileService");
    }

    ProfileService::~ProfileService() noexcept = default;

    void ProfileService::onInit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onInit";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void ProfileService::onDeinit(const std::function<void(serp::Service::Status)> reply)
    {
        logInfo() << "onDeinit";
        reply(serp::Service::Status::SUCCESSFUL);
    }

    void ProfileService::loadProfile(serp::ResponsePtr<std::string> reply, const std::string& user, const std::string& profile)
    {
        logInfo() << "loadProfile user=" << user << " profile=" << profile;
        CurrentActiveProfile = profile;
        ProfileChanged(user, profile);
        reply->call(profile);
    }

    void ProfileService::saveProfile(serp::ResponsePtr<bool> reply, const std::string& user, const std::string& profile)
    {
        logInfo() << "saveProfile user=" << user << " profile=" << profile;
        reply->call(true);
    }

    void ProfileService::activeProfile(serp::ResponsePtr<std::string> reply)
    {
        reply->call(static_cast<std::string>(CurrentActiveProfile));
    }

    void ProfileService::frame(serp::ResponsePtr<std::string> reply)
    {
        std::ostringstream out;
        out << "profile.active=" << static_cast<std::string>(CurrentActiveProfile) << "\n";
        reply->call(out.str());
    }

} // namespace Minivi
