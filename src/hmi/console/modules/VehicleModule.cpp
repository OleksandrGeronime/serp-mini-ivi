#include "ModuleUi.hpp"

namespace mini_hmi {
namespace action_id = HmiActionId;

void renderVehicle(Canvas& c, Rect r, const ViewModel&, const ScreenData& d) {
    c.box(Rect{r.x, r.y, r.w, r.h}, "VEHICLE STATUS");
    c.text(r.x + 12, r.y + 4, "       _________");
    c.text(r.x + 12, r.y + 5, "  ____/  _____  \\____");
    c.text(r.x + 12, r.y + 6, " / _    /     \\    _ \\");
    c.text(r.x + 12, r.y + 7, "| (_)  |       |  (_) |");
    c.text(r.x + 12, r.y + 8, " \\_____|_______|_____/ ");
    module_ui::field(c, r.x + 48, r.y + 4, "Ignition", d.vehicle.ignition);
    module_ui::field(c, r.x + 48, r.y + 6, "Gear", d.vehicle.gear);
    module_ui::field(c, r.x + 48, r.y + 8, "Speed", d.vehicle.speed + " mph");
    module_ui::field(c, r.x + 48, r.y + 10, "Moving", d.vehicle.moving);
}

HmiModule vehicleModule() {
    return HmiModule{Screen::Vehicle,
                     "vehicle",
                     "VEHICLE",
                     {"car"},
                     {{"1", "Ign ON", action(action_id::CarPropertySet, {"ignition", "on"})}, {"2", "Ign OFF", action(action_id::CarPropertySet, {"ignition", "off"})}, {"3", "Park", action(action_id::CarPropertySet, {"gear", "park"})}, {"4", "Drive", action(action_id::CarPropertySet, {"gear", "drive"})},
                      {"5", "0 mph", action(action_id::CarPropertySet, {"speed", "0"})}, {"6", "35 mph", action(action_id::CarPropertySet, {"speed", "35"})}, {"7", "Restrictions", action(action_id::DemoScenarioRun, {"driving-restrictions"})}},
                     renderVehicle};
}

} // namespace mini_hmi
