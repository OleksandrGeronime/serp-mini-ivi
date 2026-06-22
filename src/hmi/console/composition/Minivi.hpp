#pragma once

#include "HmiClient.hpp"

#include <memory>
#include <string>

#include "serp/serp.hpp"

// HALs
#include "IVehicleHal.hpp"
#include "IClimateHal.hpp"
#include "IPhoneHal.hpp"
#include "IRadioHal.hpp"
#include "ILocationHal.hpp"
#include "IAudioHal.hpp"
#include "IPersistenceHal.hpp"

// System services
#include "IPersistenceService.hpp"
#include "ICarNotificationManager.hpp"
#include "ICarPropertyService.hpp"
#include "ICarPowerManager.hpp"
#include "ICarUserManager.hpp"

// Cabin services
#include "ICarClimateManager.hpp"
#include "ICarNavigationManager.hpp"
#include "ICarPhoneManager.hpp"

// Media services
#include "ICarAudioManager.hpp"
#include "ICarMediaManager.hpp"
#include "ICarRadioManager.hpp"

namespace mini_hmi {

class Minivi : public IHmiClient {
public:
    void start();
    void stop();
    ScreenData snapshot() override;
    std::string dispatch(const std::string& user, const HmiAction& action) override;

private:
    void bootstrapPersistentState();

    std::shared_ptr<Minivi::IPersistenceHal>         mPersistenceHal;
    std::shared_ptr<Minivi::IAudioHal>               mAudioHal;
    std::shared_ptr<Minivi::IRadioHal>               mRadioHal;
    std::shared_ptr<Minivi::IVehicleHal>             mVehicleHal;
    std::shared_ptr<Minivi::IClimateHal>             mClimateHal;
    std::shared_ptr<Minivi::IPhoneHal>               mPhoneHal;
    std::shared_ptr<Minivi::ILocationHal>            mLocationHal;

    std::shared_ptr<Minivi::IPersistenceService>     mPersistence;
    std::shared_ptr<Minivi::ICarNotificationManager> mCarNotification;
    std::shared_ptr<Minivi::ICarPropertyService>     mCarProperty;
    std::shared_ptr<Minivi::ICarPowerManager>        mCarPower;
    std::shared_ptr<Minivi::ICarUserManager>         mCarUser;

    std::shared_ptr<Minivi::ICarClimateManager>      mCarClimate;
    std::shared_ptr<Minivi::ICarNavigationManager>   mCarNavigation;
    std::shared_ptr<Minivi::ICarPhoneManager>        mCarPhone;

    std::shared_ptr<Minivi::ICarAudioManager>        mCarAudio;
    std::shared_ptr<Minivi::ICarMediaManager>        mCarMedia;
    std::shared_ptr<Minivi::ICarRadioManager>        mCarRadio;
};

} // namespace mini_hmi
