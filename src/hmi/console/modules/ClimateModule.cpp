#include "ModuleUi.hpp"

namespace mini_hmi {
namespace action_id = HmiActionId;

void renderClimate(Canvas& c, Rect r, const ViewModel&, const ScreenData& d) {
    const int driver = module_ui::boundedTemp(d.climate.driver, 72);
    const int passenger = module_ui::boundedTemp(d.climate.passenger, 70);
    c.box(Rect{r.x, r.y, r.w, r.h}, "DUAL ZONE CLIMATE");
    c.text(r.x + 8, r.y + 3, "DRIVER");
    c.text(r.x + r.w - 25, r.y + 3, "PASSENGER");
    c.text(r.x + 7, r.y + 5, "+--------+");
    c.text(r.x + 7, r.y + 6, "|  " + std::to_string(driver) + " F  |");
    c.text(r.x + 7, r.y + 7, "+--------+");
    c.text(r.x + r.w - 27, r.y + 5, "+--------+");
    c.text(r.x + r.w - 27, r.y + 6, "|  " + std::to_string(passenger) + " F  |");
    c.text(r.x + r.w - 27, r.y + 7, "+--------+");
    c.progress(Rect{r.x + 20, r.y + 10, r.w - 40, 3}, parsePercentLike(d.climate.fan, 30), "FAN");
    c.text(r.x + 20, r.y + 15, "AUTO  A/C  SYNC  FRONT DEFROST  REAR DEFROST");
}

HmiModule climateModule() {
    return HmiModule{Screen::Climate,
                     "climate",
                     "CLIMATE",
                     {"hvac"},
                     {{"1", "Drv 68", action(action_id::CarHvacSetTemperature, {"driver", "68"})}, {"2", "Drv 72", action(action_id::CarHvacSetTemperature, {"driver", "72"})}, {"3", "Pass 70", action(action_id::CarHvacSetTemperature, {"passenger", "70"})},
                      {"4", "Pass 74", action(action_id::CarHvacSetTemperature, {"passenger", "74"})}, {"5", "Fan 2", action(action_id::CarHvacSetFan, {"2"})}, {"6", "Fan 5", action(action_id::CarHvacSetFan, {"5"})}},
                     renderClimate};
}

} // namespace mini_hmi
