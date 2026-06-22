#include "ModuleUi.hpp"

namespace mini_hmi {
namespace action_id = HmiActionId;

void renderNotifications(Canvas& c, Rect r, const ViewModel&, const ScreenData& d) {
    c.box(r, "NOTIFICATION CENTER");
    int y = r.y + 2;
    for (const auto& note : d.notifications) {
        c.box(Rect{r.x + 3, y, r.w - 6, 3}, "");
        c.text(r.x + 5, y + 1, clampText(note, r.w - 10));
        y += 4;
        if (y >= r.y + r.h - 3) break;
    }
    if (d.notifications.empty()) c.text(r.x + 5, r.y + 3, "No active notifications.");
}

HmiModule notificationsModule() {
    return HmiModule{Screen::Notifications,
                     "notifications",
                     "NOTIFICATIONS",
                     {"notifs", "notification"},
                     {{"1", "Dismiss 1", action(action_id::NotificationCenterDismiss, {"1"})}, {"2", "Refresh", action(action_id::SnapshotRefresh)}, {"3", "Home", action("screen.open", {"home"})}},
                     renderNotifications};
}

} // namespace mini_hmi
