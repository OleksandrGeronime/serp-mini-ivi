#include "ModuleUi.hpp"

#include <array>

namespace mini_hmi {
namespace action_id = HmiActionId;

// ── helpers ──────────────────────────────────────────────────────────────────

static void renderContactRow(Canvas& c, int x, int y, int w,
                             const std::string& name, const std::string& number,
                             const std::string& callActionId) {
    const std::string label = clampText(name.empty() ? number : name, w - 24);
    const std::string num   = clampText(number, 16);
    c.text(x + 1, y, label);
    c.text(x + 1 + static_cast<int>(label.size()) + 1, y, num);
    c.button(Rect{x + w - 8, y - 1, 8, 3}, callActionId, "CALL");
}

static void renderContactList(Canvas& c, Rect r,
                              const std::vector<ContactItem>& items,
                              const std::string& emptyMsg,
                              const std::string& actionPrefix) {
    int y = r.y + 1;
    if (items.empty()) {
        c.text(r.x + 2, y, emptyMsg);
        return;
    }
    for (std::size_t i = 0; i < items.size() && y < r.y + r.h - 2; ++i, y += 2) {
        const auto& contact = items[i];
        const std::string id = actionPrefix + std::to_string(i);
        renderContactRow(c, r.x + 1, y, r.w - 2, contact.name, contact.number, id);
        c.addTarget(Rect{r.x + 1, y - 1, r.w - 10, 3}, id);
    }
}

// ── Phone Main ────────────────────────────────────────────────────────────────

void renderPhone(Canvas& c, Rect r, const ViewModel& model, const ScreenData& d) {
    const auto& ph = d.phone;

    // tab headers
    const int tabW = (r.w - 12) / 3;
    const std::array<std::string, 3> tabs{"FAVORITES", "RECENTS", "CONTACTS"};
    const std::array<std::string, 3> tabIds{"favorites", "recents", "contacts"};
    for (int i = 0; i < 3; ++i) {
        const bool sel = ph.activeTab == tabIds[static_cast<std::size_t>(i)];
        const std::string label = sel ? ("[ " + tabs[static_cast<std::size_t>(i)] + " ]")
                                      : ("  " + tabs[static_cast<std::size_t>(i)] + "  ");
        const int tx = r.x + i * (tabW + 1);
        c.text(tx, r.y, clampText(label, tabW + 2));
        c.addTarget(Rect{tx, r.y, tabW + 2, 1},
                    "phone.tab " + tabIds[static_cast<std::size_t>(i)]);
    }
    c.button(Rect{r.x + r.w - 10, r.y, 10, 3}, "screen.open phone-dialer", "DIAL");
    c.hline(r.x, r.y + 1, r.w - 11, '-');

    const Rect body{r.x, r.y + 2, r.w, r.h - 2};

    if (ph.activeTab == "recents") {
        c.box(body, "RECENTS");
        int y = body.y + 2;
        if (ph.recentCalls.empty()) {
            c.text(body.x + 2, y, "No recent calls.");
        } else {
            for (std::size_t i = 0; i < ph.recentCalls.size() && y < body.y + body.h - 2; ++i, y += 2) {
                const auto& rc = ph.recentCalls[i];
                const std::string dir = (rc.direction == "incoming") ? "<-"
                                      : (rc.direction == "outgoing") ? "->"
                                      : " X";
                const std::string label = dir + " " + clampText(rc.name.empty() ? rc.number : rc.name, 16)
                                        + "  " + clampText(rc.timestamp, 8)
                                        + "  " + rc.duration;
                c.text(body.x + 2, y, clampText(label, body.w - 12));
                const std::string id = "phone.dial.recent." + std::to_string(i);
                c.button(Rect{body.x + body.w - 10, y - 1, 8, 3}, id, "CALL");
                c.addTarget(Rect{body.x + 1, y - 1, body.w - 11, 3}, id);
            }
        }
    } else if (ph.activeTab == "contacts") {
        c.box(body, "CONTACTS");
        const std::string searchLabel = ph.contactSearchQuery.empty()
            ? "[ Search... ]" : "[ " + ph.contactSearchQuery + "_ ]";
        c.text(body.x + 2, body.y + 1, clampText(searchLabel, body.w - 4));
        c.addTarget(Rect{body.x + 1, body.y + 1, body.w - 2, 1}, "screen.open phone-contact-search");
        const auto& list = ph.contactSearchQuery.empty() ? ph.contacts : ph.contactSearchResults;
        renderContactList(c, Rect{body.x, body.y + 2, body.w, body.h - 2},
                          list, "No contacts.", "phone.dial.contact.");
    } else {
        c.box(body, "FAVORITES");
        renderContactList(c, body, ph.favoriteContacts, "No favorites.", "phone.dial.fav.");
    }
}

// ── Phone Dialer ──────────────────────────────────────────────────────────────

void renderPhoneDialer(Canvas& c, Rect r, const ViewModel& model, const ScreenData& d) {
    const auto& ph = d.phone;
    const int padW = r.w * 6 / 10;
    const int matchX = r.x + padW + 2;
    const int matchW = r.w - padW - 2;

    // input field
    c.box(Rect{r.x, r.y, padW, 3}, "INPUT");
    const std::string inputDisplay = ph.dialQuery + "_";
    c.text(r.x + 2, r.y + 1, clampText(inputDisplay, padW - 10));
    c.button(Rect{r.x + padW - 9, r.y, 5, 3}, "phone.dial_backspace", "<-");
    c.button(Rect{r.x + padW - 4, r.y, 4, 3}, "phone.dial_clear", "CLR");

    // numpad
    const std::array<std::string, 12> keys{"1","2","3","4","5","6","7","8","9","*","0","#"};
    const int keyW = (padW - 4) / 3;
    const int keyH = 3;
    for (int row = 0; row < 4; ++row) {
        for (int col = 0; col < 3; ++col) {
            const std::size_t idx = static_cast<std::size_t>(row * 3 + col);
            const std::string key = keys[idx];
            const int kx = r.x + 1 + col * (keyW + 1);
            const int ky = r.y + 4 + row * (keyH + 1);
            c.button(Rect{kx, ky, keyW, keyH}, "phone.dial_key " + key, key);
        }
    }

    // CALL / BACK
    const int btnY = r.y + 4 + 4 * (keyH + 1) + 1;
    c.button(Rect{r.x + 1, btnY, padW / 2 - 2, 3}, "telecom.call.dial", "CALL");
    c.button(Rect{r.x + padW / 2 + 1, btnY, padW / 2 - 2, 3}, "screen.back", "BACK");

    // match list
    c.box(Rect{matchX, r.y, matchW, r.h}, "MATCHES");
    int my = r.y + 2;
    if (ph.dialFilteredContacts.empty()) {
        c.text(matchX + 2, my, ph.dialQuery.empty() ? "Type to filter" : "No matches");
    } else {
        for (std::size_t i = 0; i < ph.dialFilteredContacts.size() && my < r.y + r.h - 1; ++i, my += 2) {
            const auto& fc = ph.dialFilteredContacts[i];
            const std::string id = "phone.dial.filtered." + std::to_string(i);
            c.text(matchX + 2, my, clampText(fc.name, matchW / 2 - 2));
            c.text(matchX + matchW / 2, my, clampText(fc.number, matchW / 2 - 2));
            c.addTarget(Rect{matchX + 1, my - 1, matchW - 2, 3}, id);
        }
    }
}

// ── Phone Active Call ─────────────────────────────────────────────────────────

void renderPhoneActiveCall(Canvas& c, Rect r, const ViewModel& model, const ScreenData& d) {
    const auto& ph = d.phone;
    const int cx = r.x + r.w / 4;
    const int cw = r.w / 2;
    const int cy = r.y + 1;

    c.box(Rect{cx, cy, cw, r.h - 2}, "");

    if (ph.state == "dialing") {
        c.centered(cy + 2, "-> Calling...");
        const std::string who = ph.contactName.empty() ? ph.number : ph.contactName;
        c.centered(cy + 4, clampText(who, cw - 4));
        c.centered(cy + 5, clampText(ph.number, cw - 4));
        c.button(Rect{cx + cw / 2 - 5, cy + r.h - 6, 10, 3}, action_id::TelecomCallEnd, "END");

    } else if (ph.state == "ringing") {
        c.centered(cy + 2, "<- Incoming call");
        const std::string who = ph.contactName.empty() ? ph.number : ph.contactName;
        c.centered(cy + 4, clampText(who, cw - 4));
        c.centered(cy + 5, clampText(ph.number, cw - 4));
        c.button(Rect{cx + 2, cy + r.h - 6, cw / 2 - 4, 3}, action_id::TelecomCallAccept, "ACCEPT");
        c.button(Rect{cx + cw / 2 + 2, cy + r.h - 6, cw / 2 - 4, 3}, action_id::TelecomCallDecline, "DECLINE");

    } else if (ph.state == "active") {
        const std::string dir = (ph.direction == "incoming") ? "<-" : "->";
        c.centered(cy + 2, dir + " On Call  " + (ph.callDuration.empty() ? "00:00" : ph.callDuration));
        const std::string who = ph.contactName.empty() ? ph.number : ph.contactName;
        c.centered(cy + 4, clampText(who, cw - 4));
        c.centered(cy + 5, clampText(ph.number, cw - 4));
        const int btnY = cy + r.h - 6;
        const int btnW = (cw - 8) / 3;
        c.button(Rect{cx + 2,              btnY, btnW, 3}, action_id::TelecomCallEnd,   "END");
        const std::string muteLabel = ph.muteActive ? "UNMUTE" : "MUTE";
        const std::string muteId    = ph.muteActive ? action_id::TelecomCallUnmute : action_id::TelecomCallMute;
        c.button(Rect{cx + 3 + btnW,       btnY, btnW, 3}, muteId,                      muteLabel);
        c.button(Rect{cx + 4 + btnW * 2,   btnY, btnW, 3}, "phone.open_dialer_dtmf",    "KEYPAD");

    } else {
        c.centered(cy + r.h / 2, "Call ended.");
    }
}

// ── Phone Contact Search ──────────────────────────────────────────────────────

void renderPhoneContactSearch(Canvas& c, Rect r, const ViewModel& model, const ScreenData& d) {
    const auto& ph = d.phone;

    // search field
    c.box(Rect{r.x, r.y, r.w, 3}, "SEARCH");
    const std::string searchDisplay = ph.contactSearchQuery + "_";
    c.text(r.x + 2, r.y + 1, clampText(searchDisplay, r.w - 10));
    c.button(Rect{r.x + r.w - 9, r.y, 5, 3}, "phone.search_backspace", "<-");
    c.button(Rect{r.x + r.w - 4, r.y, 4, 3}, "phone.search_clear", "CLR");

    // QWERTY keyboard
    const std::array<std::string, 10> row1{"Q","W","E","R","T","Y","U","I","O","P"};
    const std::array<std::string, 9>  row2{"A","S","D","F","G","H","J","K","L"};
    const std::array<std::string, 7>  row3{"Z","X","C","V","B","N","M"};
    const int keyW = std::max(4, (r.w - 4) / 10);
    const int keyH = 3;

    auto drawRow = [&](int ky, auto& keys, int offset) {
        for (std::size_t i = 0; i < keys.size(); ++i) {
            const int kx = r.x + 1 + offset + static_cast<int>(i) * (keyW + 1);
            c.button(Rect{kx, ky, keyW, keyH}, "phone.search_key " + keys[i], keys[i]);
        }
    };
    const int kbY = r.y + 4;
    drawRow(kbY,               row1, 0);
    drawRow(kbY + keyH + 1,    row2, keyW / 2);
    drawRow(kbY + (keyH+1)*2,  row3, keyW);

    // SPACE and BACK
    const int lastRowY = kbY + (keyH + 1) * 3;
    c.button(Rect{r.x + 1,          lastRowY, r.w / 2 - 2, keyH}, "phone.search_key  ", "SPACE");
    c.button(Rect{r.x + r.w / 2 + 1, lastRowY, r.w / 2 - 2, keyH}, "screen.back",        "BACK");

    // results below keyboard
    const int resY = lastRowY + keyH + 2;
    if (resY < r.y + r.h - 2) {
        c.box(Rect{r.x, resY, r.w, r.y + r.h - resY}, "RESULTS");
        renderContactList(c, Rect{r.x, resY, r.w, r.y + r.h - resY},
                          ph.contactSearchResults, "No results.", "phone.dial.search.");
    }
}

// ── module factories ──────────────────────────────────────────────────────────

HmiModule phoneModule() {
    return HmiModule{Screen::Phone,
                     "phone",
                     "PHONE",
                     {},
                     {},
                     renderPhone};
}

HmiModule phoneDialerModule() {
    return HmiModule{Screen::PhoneDialer,
                     "phone-dialer",
                     "DIALER",
                     {},
                     {},
                     renderPhoneDialer,
                     true};
}

HmiModule phoneActiveCallModule() {
    return HmiModule{Screen::PhoneActiveCall,
                     "phone-active-call",
                     "ACTIVE CALL",
                     {},
                     {},
                     renderPhoneActiveCall,
                     true};
}

HmiModule phoneContactSearchModule() {
    return HmiModule{Screen::PhoneContactSearch,
                     "phone-contact-search",
                     "CONTACT SEARCH",
                     {},
                     {},
                     renderPhoneContactSearch,
                     true};
}

} // namespace mini_hmi
