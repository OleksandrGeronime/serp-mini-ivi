#include "ModuleUi.hpp"

namespace mini_hmi {
namespace action_id = HmiActionId;

void renderSettings(Canvas& c, Rect r, const ViewModel&, const ScreenData& d) {
    c.box(Rect{r.x, r.y, r.w, r.h}, "SETTINGS");
    const std::vector<std::string> rows{
        "Theme        " + d.settings.theme,
        "Units        " + d.settings.units,
        "Brightness   " + d.settings.brightness,
        "Layout       " + d.settings.layout,
        "Safety lock  enabled while moving"};
    int y = r.y + 3;
    for (const auto& row : rows) {
        c.text(r.x + 5, y, "> " + clampText(row, r.w - 10));
        y += 2;
    }
    c.text(r.x + 5, r.y + r.h - 3, "Values are saved to file-backed PersistenceService.");
}

HmiModule settingsModule() {
    return HmiModule{Screen::Settings,
                     "settings",
                     "SETTINGS",
                     {},
                     {{"1", "Dark", action(action_id::CarUserSettingsSet, {"theme", "dark"})}, {"2", "Light", action(action_id::CarUserSettingsSet, {"theme", "light"})}, {"3", "Imperial", action(action_id::CarUserSettingsSet, {"units", "imperial"})}, {"4", "Metric", action(action_id::CarUserSettingsSet, {"units", "metric"})},
                      {"5", "Bright 65", action(action_id::CarUserSettingsSet, {"brightness", "65"})}, {"6", "Bright 90", action(action_id::CarUserSettingsSet, {"brightness", "90"})}, {"7", "Split", action(action_id::CarUserSettingsSet, {"layout", "split"})}, {"8", "Passenger", action(action_id::CarUserSettingsSet, {"layout", "passenger"})}},
                     renderSettings};
}

} // namespace mini_hmi
