#include "ModuleUi.hpp"

namespace mini_hmi {
namespace action_id = HmiActionId;

void renderHome(Canvas& c, Rect r, const ViewModel&, const ScreenData& d) {
    const int colW = (r.w - 4) / 2;
    module_ui::card(c, Rect{r.x, r.y, colW, 7}, "NOW PLAYING", {
        "State  " + d.media.state,
        "Track  " + d.media.track,
        "Focus  " + d.focus.owner + " / " + d.focus.mode});
    module_ui::card(c, Rect{r.x + colW + 2, r.y, r.w - colW - 2, 7}, "ROUTE", {
        "State       " + d.nav.state,
        "Destination " + d.nav.destination,
        "Prompt path through AudioFocus"});
    module_ui::card(c, Rect{r.x, r.y + 8, colW, 7}, "CABIN", {
        "Driver temp     " + d.climate.driver,
        "Passenger temp  " + d.climate.passenger,
        "Fan             " + d.climate.fan});
    module_ui::card(c, Rect{r.x + colW + 2, r.y + 8, r.w - colW - 2, 7}, "VEHICLE", {
        "Gear    " + d.vehicle.gear,
        "Speed   " + d.vehicle.speed + " mph",
        "Moving  " + d.vehicle.moving});
}

HmiModule homeModule() {
    return HmiModule{Screen::Home,
                     "home",
                     "HOME",
                     {},
                     {{"1", "Media", action("screen.open", {"media"})}, {"2", "Nav", action("screen.open", {"nav"})}, {"3", "Phone", action("screen.open", {"phone"})}, {"4", "Climate", action("screen.open", {"climate"})},
                      {"5", "Settings", action("screen.open", {"settings"})}, {"6", "Profile", action("screen.open", {"profile"})}, {"7", "Media demo", action(action_id::DemoScenarioRun, {"media-basic"})}, {"8", "Nav demo", action(action_id::DemoScenarioRun, {"nav-ducking"})}},
                     renderHome};
}

} // namespace mini_hmi
