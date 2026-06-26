#include "HmiModules.hpp"

namespace mini_hmi {

const std::vector<HmiModule>& hmiModules() {
    static const std::vector<HmiModule> modules{
        homeModule(),
        mediaModule(),
        radioModule(),
        navigationModule(),
        phoneModule(),
        climateModule(),
        settingsModule(),
        profileModule(),
        vehicleModule(),
        notificationsModule(),
        phoneDialerModule(),
        phoneActiveCallModule(),
        phoneContactSearchModule(),
    };
    return modules;
}

const HmiModule* hmiModule(Screen screen) {
    for (const auto& module : hmiModules()) {
        if (module.screen == screen) return &module;
    }
    return nullptr;
}

const HmiModule* hmiModuleById(const std::string& idOrAlias) {
    const auto value = lowerCopy(idOrAlias);
    for (const auto& module : hmiModules()) {
        if (module.id == value) return &module;
        for (const auto& alias : module.aliases) {
            if (alias == value) return &module;
        }
    }
    return nullptr;
}

} // namespace mini_hmi
