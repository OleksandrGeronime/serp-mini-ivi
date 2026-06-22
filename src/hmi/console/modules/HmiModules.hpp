#pragma once

#include "HmiScreens.hpp"
#include "HmiTypes.hpp"
#include "UiPrimitives.hpp"

#include <string>
#include <vector>

namespace mini_hmi {

using ModuleRenderFn = void (*)(Canvas&, Rect, const ViewModel&, const ScreenData&);

struct HmiModule {
    Screen screen;
    std::string id;
    std::string title;
    std::vector<std::string> aliases;
    std::vector<ButtonSpec> buttons;
    ModuleRenderFn render;
};

const std::vector<HmiModule>& hmiModules();
const HmiModule* hmiModule(Screen screen);
const HmiModule* hmiModuleById(const std::string& idOrAlias);

HmiModule homeModule();
HmiModule mediaModule();
HmiModule radioModule();
HmiModule navigationModule();
HmiModule phoneModule();
HmiModule climateModule();
HmiModule settingsModule();
HmiModule profileModule();
HmiModule vehicleModule();
HmiModule notificationsModule();

} // namespace mini_hmi
