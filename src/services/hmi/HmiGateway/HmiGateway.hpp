// Product-owned SERP service implementation scaffold.
// The generator creates this file only if it is missing and never overwrites it.

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <tuple>

#include "HmiGatewayBase.hpp"
#include "serp/serp.hpp"

namespace Minivi
{
    class HmiGateway : public HmiGatewayBase
    {
    public:
        explicit HmiGateway(std::shared_ptr<IMediaService> mediaservice, std::shared_ptr<IRadioService> radioservice, std::shared_ptr<IAudioFocusService> audiofocusservice, std::shared_ptr<INavigationService> navigationservice, std::shared_ptr<IPhoneService> phoneservice, std::shared_ptr<IClimateService> climateservice, std::shared_ptr<IProfileService> profileservice, std::shared_ptr<IVehicleStateService> vehiclestateservice, std::shared_ptr<ISettingsService> settingsservice, std::shared_ptr<INotificationService> notificationservice, std::shared_ptr<IAudioHal> audiohal);
        ~HmiGateway() noexcept override;

    protected:
        void onInit(const std::function<void(serp::Service::Status)> reply) override;
        void onDeinit(const std::function<void(serp::Service::Status)> reply) override;

        void dispatch(serp::ResponsePtr<std::string> reply, const std::string& user, const std::string& actionId, const std::string& args) override;
        void snapshot(serp::ResponsePtr<std::string> reply) override;
        void frame(serp::ResponsePtr<std::string> reply) override;

    private:
        void refreshSnapshot();

        serp::TimerPtr mSnapshotTimer;
        std::string mCachedRadioFrame;
        std::string mCachedMediaFrame;
        std::string mCachedAudioFocusFrame;
        std::string mCachedNavFrame;
        std::string mCachedPhoneFrame;
        std::string mCachedClimateFrame;
        std::string mCachedProfileFrame;
        std::string mCachedVehicleFrame;
    };

} // namespace Minivi
