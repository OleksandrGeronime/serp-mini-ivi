#pragma once

#include "HmiClient.hpp"

#include <chrono>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>

#include "serp/serp.hpp"

#include "ICarPropertyService.hpp"
#include "ICarNotificationManager.hpp"
#include "ICarClimateManager.hpp"
#include "ICarNavigationManager.hpp"
#include "ICarCallManager.hpp"
#include "ICarContactsManager.hpp"
#include "ICarUserManager.hpp"
#include "ICarMediaManager.hpp"
#include "ICarRadioManager.hpp"

namespace mini_hmi {

class RemoteHmiClient : public IHmiClient {
public:
    void start();
    void stop();
    ScreenData snapshot() override;
    std::string dispatch(const std::string& user, const HmiAction& action) override;

private:
    template<typename T>
    static std::string toStr(const T& v) {
        std::ostringstream s; s << v; return s.str();
    }

    std::shared_ptr<serp::transport::Backend>             mBackend;

    std::shared_ptr<Minivi::ICarPropertyService>     mCarProperty;
    std::shared_ptr<Minivi::ICarNotificationManager> mCarNotification;
    std::shared_ptr<Minivi::ICarClimateManager>      mCarClimate;
    std::shared_ptr<Minivi::ICarNavigationManager>   mCarNavigation;
    std::shared_ptr<Minivi::ICarCallManager>         mCarCall;
    std::shared_ptr<Minivi::ICarContactsManager>     mCarContacts;
    std::shared_ptr<Minivi::ICarUserManager>         mCarUser;
    std::shared_ptr<Minivi::ICarMediaManager>        mCarMedia;
    std::shared_ptr<Minivi::ICarRadioManager>        mCarRadio;

    std::chrono::steady_clock::time_point mCallStartedAt{};
    bool mCallTimerActive{false};

    // Notification state
    mutable std::mutex mNotifMutex;
    struct NotifEntry { int32_t id; std::string text; };
    std::vector<NotifEntry> mNotifCache;
    uint32_t mNotifPostedSubId{0};
    uint32_t mNotifDismissedSubId{0};

    // Generic settings cache (from SettingChanged notification)
    mutable std::mutex mSettingsMutex;
    std::map<std::string, std::string> mSettingsCache;
    uint32_t mSettingChangedSubId{0};

    ScreenData  mLastSnapshot;
    std::string mLastDispatchResult;
};

} // namespace mini_hmi
