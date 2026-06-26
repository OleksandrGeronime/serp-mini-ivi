# Phone Screen — Interface Requirements

## Context

This document captures what the HMI needs from backend interfaces to implement the full Phone screen feature set. It is intended for the architect session to:
- decide how to split or restructure the phone-related interfaces
- define new types and services
- consider a separate Phone application
- produce updated or new `.sidl` specs

The current `ICarPhoneManager` is out of scope for direct edits here — only requirements are captured.

---

## HMI Screens

### 1. Phone Main (3 tabs)

| Tab | What is shown |
|-----|---------------|
| **Favorites** | List of starred contacts. Each row: name, number, [CALL] button. |
| **Recents** | Call history list. Each row: direction icon (→ out / ← in / ✗ missed), name or number, timestamp, duration, [CALL] button. |
| **Contacts** | Full contact list + search field. Tap on search field → opens Contact Search screen. Each row: name, number, [CALL] button. Tap on row → `dial(number)` immediately. |

Tab switch = tap on tab header. Tap [CALL] or tap contact row = `dial(number)`.

---

### 2. Phone Dialer

Opened by tapping [DIAL] on Phone Main.

| Area | What is shown |
|------|---------------|
| Input field | Accumulated digit string, with backspace [⌫] and clear [CLR] buttons |
| Numpad | 12 buttons: 1–9, 0, \*, \# — each is a tap target |
| Match list (right side) | Contacts whose number contains the current input string, updated live after every tap |
| CALL button | Dials the current input string |
| BACK button | Returns to previous screen |

Flow:
- Numpad tap → `setDialQuery(currentInput)` → backend updates `dialFilteredContacts` → HMI redraws list
- Tap on contact in match list → `dial(contact.number)` immediately
- Tap CALL → `dial(dialQuery)`

---

### 3. Phone Contact Search

Opened by tapping the search field in the Contacts tab.

| Area | What is shown |
|------|---------------|
| Search field | Accumulated text, [⌫] and [CLR] buttons |
| Virtual keyboard | Full QWERTY + space — each key is a tap target |
| Results list | Contacts matching the query, shown below the keyboard |

Flow:
- Key tap → `setContactSearchQuery(currentText)` → backend updates `contactSearchResults` → HMI redraws list
- Tap on contact in results → `dial(contact.number)` immediately
- BACK → Contacts tab

---

### 4. Phone Active Call

Auto-shown when `currentCallState` changes to `dialing`, `ringing`, or `active` **while the user is on any Phone screen** (Main, Dialer, or Contact Search). On other screens a modal badge is planned separately (out of scope here).

| State | Shown |
|-------|-------|
| **Outgoing — dialing** | "Calling…", contact name (or raw number if not in contacts), [END] |
| **Incoming — ringing** | "Incoming call", contact name, number, [ACCEPT] [DECLINE] |
| **Active (any direction)** | "On Call", contact name, number, live duration timer, [END] [MUTE] [KEYPAD] |
| **Ended** | Brief "Call ended" banner, auto-return to Phone Main |

`[KEYPAD]` — opens Dialer in DTMF mode: same numpad UI, each tap sends `sendDtmf(key)` instead of building a dial string.

Call direction is displayed (arrow icon, label) but does not change available actions once the call is active.

---

## Interface Requirements

### New types needed

```
CallDirection: enum
  incoming
  outgoing
  missed

CallRecord: struct
  name: String          -- resolved contact name, or empty if unknown
  number: String
  direction: CallDirection
  timestamp: String     -- ISO-8601 or epoch string
  durationSec: UInt32   -- 0 for missed / unanswered
```

`Contact` (from `common/contacts.sidl`) — currently has `name` and `number`. No extension required for this feature set.

---

### Properties the HMI needs to read

| Property | Type | Status |
|----------|------|--------|
| `contacts` | `list<Contact>` | already on ICarPhoneManager |
| `currentCallState` | `CallState` | already — but needs `dialing` added (see below) |
| `phoneNumber` | `String` | already |
| `contactName` | `String` | already |
| `callDirection` | `CallDirection` | **new** |
| `muteActive` | `Boolean` | **new** |
| `favoriteContacts` | `list<Contact>` | **new** |
| `recentCalls` | `list<CallRecord>` | **new** |
| `dialQuery` | `String` | **new** — mirror of last value HMI set |
| `dialFilteredContacts` | `list<Contact>` | **new** |
| `contactSearchQuery` | `String` | **new** |
| `contactSearchResults` | `list<Contact>` | **new** |

---

### Methods the HMI needs to call

| Method | Signature | Status |
|--------|-----------|--------|
| `dial` | `(number: String) -> ok: Boolean` | already exists |
| `acceptCall` | `() -> ok: Boolean` | already exists |
| `endCall` | `() -> ok: Boolean` | already exists — also used for active call End |
| `declineCall` | `() -> ok: Boolean` | **new** — semantically distinct from ending active call |
| `muteCall` | `() -> ok: Boolean` | **new** |
| `unmuteCall` | `() -> ok: Boolean` | **new** |
| `sendDtmf` | `(key: String) -> ok: Boolean` | **new** — DTMF tone during active call |
| `setDialQuery` | `(query: String) -> ok: Boolean` | **new** |
| `setContactSearchQuery` | `(query: String) -> ok: Boolean` | **new** |

---

### Notifications the HMI needs to receive

| Notification | Signature | Status |
|--------------|-----------|--------|
| `callChanged` | `(state: CallState, number: String)` | already exists — HMI uses this to auto-navigate to Active Call screen |
| call direction on state change | to be decided | **new** — see Architect Q-D below |

---

## Questions for the Architect Session

### A. Interface split

`ICarPhoneManager` currently mixes call control with contacts. Recommended split:

- **ICarCallManager** — call state, dial, accept, end, decline, mute, DTMF. Thin proxy to phone HAL / BT stack.
- **ICarContactsManager** — full contacts, favorites, recents, dial filter, text search. Owned by the phone application.

Alternative: keep everything in one extended `ICarPhoneManager`. Simpler, but large.

Architect to decide: 2-interface split vs single interface vs 3-way split (add separate `ICarFavoritesManager` if favorites need user-profile scoping).

---

### B. Separate Phone Application

The phone feature set (BT pairing, contacts sync from paired device, call history persistence, favorites storage) is large enough to warrant a dedicated `CarPhoneApp` process.

Questions:
- Should a `CarPhoneApp` own contacts/history/favorites and expose them via `ICarContactsManager`?
- Should `CarCallManager` be a thin proxy to `IPhoneHal` and nothing more?
- Who owns persistence of `recentCalls` and `favoriteContacts` — phone app, user profile service, or a new storage service?

---

### C. `CallState` extension

Current `CallState`: `idle`, `ringing`, `active`, `ended`.

HMI needs `dialing` to show the outgoing "Calling…" state. Architect to confirm whether to extend `CallState` and whether this is a breaking change for existing subscribers.

---

### D. `callDirection` delivery

HMI needs incoming vs outgoing to display the correct UI. Options:
- Add `direction: CallDirection` to `callChanged` notification
- Add `callDirection` as a standalone property
- Derive from context (HMI called `dial()` → outgoing; ringing arrived without HMI action → incoming) — fragile, not recommended

---

### E. Contact filtering responsibility

This spec assumes **server-side filtering**: HMI sends a query string, backend returns a filtered list. This keeps the HMI stateless and works for large contact books.

Alternative: HMI receives the full contact list and filters locally. Simpler backend, but the HMI state model grows.

Architect to decide.

---

## Summary — new SIDL work needed

| Item | Action |
|------|--------|
| `CallDirection` enum | new — common or phone service |
| `CallRecord` struct | new — common or phone service |
| `ICarCallManager` | new interface — call control |
| `ICarContactsManager` | new interface — contacts, search, filter, favorites, recents |
| `ICarPhoneManager` | deprecate or restructure once above are defined |
| `CallState` — add `dialing` value | extend |
| `HmiApp.sidl` `uses:` | update to reference new interfaces once specs are ready |
