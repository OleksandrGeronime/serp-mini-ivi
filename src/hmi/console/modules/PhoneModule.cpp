#include "ModuleUi.hpp"

#include <array>

namespace mini_hmi {
namespace action_id = HmiActionId;

void renderPhone(Canvas& c, Rect r, const ViewModel&, const ScreenData& d) {
    module_ui::card(c, Rect{r.x, r.y, r.w / 2 - 1, 9}, "CALL", {
        "State : " + d.phone.state,
        "Number: " + d.phone.number,
        "Focus : phone priority > nav > media"});
    c.box(Rect{r.x + r.w / 2 + 1, r.y, r.w / 2 - 1, 14}, "DIAL PAD");
    const std::array<std::string, 12> keys{"1", "2 ABC", "3 DEF", "4 GHI", "5 JKL", "6 MNO", "7 PQRS", "8 TUV", "9 WXYZ", "*", "0 +", "#"};
    int i = 0;
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 3; ++col) {
            c.text(r.x + r.w / 2 + 5 + col * 10, r.y + 3 + row * 2, "[" + clampText(keys[static_cast<std::size_t>(i++)], 6) + "]");
        }
    }
    module_ui::card(c, Rect{r.x, r.y + 11, r.w / 2 - 1, 7}, "RECENTS", {"Alex", "Service", "+1-555-0101"});
}

HmiModule phoneModule() {
    return HmiModule{Screen::Phone,
                     "phone",
                     "PHONE",
                     {},
                     {{"1", "Incoming", action(action_id::TelecomCallSimulateIncoming, {"+1-555-0101"})}, {"2", "Accept", action(action_id::TelecomCallAccept)}, {"3", "End", action(action_id::TelecomCallEnd)}, {"4", "Call demo", action(action_id::DemoScenarioRun, {"phone-call"})}},
                     renderPhone};
}

} // namespace mini_hmi
