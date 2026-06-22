#include "ModuleUi.hpp"

namespace mini_hmi {
namespace action_id = HmiActionId;

void renderProfile(Canvas& c, Rect r, const ViewModel& model, const ScreenData& d) {
    c.box(Rect{r.x, r.y, 26, 12}, "ACTIVE PROFILE");
    c.text(r.x + 8, r.y + 3, "  O  ");
    c.text(r.x + 8, r.y + 4, " /|\\ ");
    c.text(r.x + 8, r.y + 5, " / \\ ");
    c.text(r.x + 4, r.y + 8, "Seat: " + toString(model.seat));
    c.text(r.x + 4, r.y + 9, "Name: " + d.profile.active);
    module_ui::card(c, Rect{r.x + 28, r.y, r.w - 28, 12}, "APPLY SEQUENCE", {
        "[x] load profile from persistence",
        "[x] apply settings",
        "[x] apply climate zones",
        "[x] update media/radio preferences",
        "[x] notify HMI progress"});
}

HmiModule profileModule() {
    return HmiModule{Screen::Profile,
                     "profile",
                     "PROFILE",
                     {},
                     {{"1", "Load Alex", action(action_id::CarUserProfileLoad, {"alex"})}, {"2", "Save Alex", action(action_id::CarUserProfileSave, {"alex"})}, {"3", "Driver", action("seat.switch", {"driver"})}, {"4", "CoDriver", action("seat.switch", {"codriver"})}},
                     renderProfile};
}

} // namespace mini_hmi
