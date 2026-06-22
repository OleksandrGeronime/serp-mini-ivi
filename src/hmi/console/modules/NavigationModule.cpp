#include "ModuleUi.hpp"

#include <algorithm>

namespace mini_hmi {
namespace action_id = HmiActionId;

void renderNavigation(Canvas& c, Rect r, const ViewModel&, const ScreenData& d) {
    const int mapW = std::max(38, r.w - 34);
    c.box(Rect{r.x, r.y, mapW, r.h}, "MAP");
    for (int y = r.y + 2; y < r.y + r.h - 1; y += 2) c.hline(r.x + 2, y, mapW - 4, '.');
    for (int x = r.x + 4; x < r.x + mapW - 2; x += 8) c.vline(x, r.y + 1, r.h - 2, ':');
    c.text(r.x + 8, r.y + 4, "START o=====>=====>=====> DEST");
    c.text(r.x + 22, r.y + 8, "turn right in 0.4 mi");
    module_ui::card(c, Rect{r.x + mapW + 2, r.y, r.w - mapW - 2, r.h}, "GUIDANCE", {
        "State: " + d.nav.state,
        "To   : " + d.nav.destination,
        "Prompt uses transient focus",
        "Media is ducked then restored",
        "CoDriver can search; driver confirms"});
}

HmiModule navigationModule() {
    return HmiModule{Screen::Navigation,
                     "navigation",
                     "NAVIGATION",
                     {"nav"},
                     {{"1", "Home", action(action_id::NavigationGuidanceStart, {"Home"})}, {"2", "Work", action(action_id::NavigationGuidanceStart, {"Work"})}, {"3", "Coffee", action(action_id::NavigationGuidanceStart, {"Coffee"}), false, true}, {"4", "Prompt", action(action_id::NavigationGuidancePrompt)},
                      {"5", "Cancel", action(action_id::NavigationGuidanceCancel)}, {"6", "Duck demo", action(action_id::DemoScenarioRun, {"nav-ducking"})}},
                     renderNavigation};
}

} // namespace mini_hmi
