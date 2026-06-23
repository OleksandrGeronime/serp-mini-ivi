# SERP Minivi

A Mini IVI (In-Vehicle Infotainment) platform built on the [SERP](https://github.com/OleksandrGeronime/serp) framework.

Minivi is a full automotive middleware stack — climate, media, navigation, phone, notifications, vehicle state — generated from specs and running as a multiprocess system over DBus or gRPC.

## Layout

```text
specs/
  serp.sidl          # workspace entry point
  services/          # service and interface declarations
  common/            # shared SIDL types
  deployments/       # deployment variants (monolith, multiprocess-dbus, multiprocess-dbus-qtquick)

gen/                 # generated — do not edit
  services/          # interfaces, base classes, mocks, CMake targets
  deployments/       # process entrypoints, IPC adapters/proxies, launchers

src/                 # handwritten — survives regeneration
  services/          # business logic implementations
  hmi/
    console/         # terminal HMI frontend
    qtquick/         # Qt Quick HMI frontend

scripts/
  generate.sh        # regenerate workspace
  build.sh           # build a deployment variant
  run.sh             # run an already-built variant
  stop.sh            # stop running processes
```

## Prerequisites

- **SERP SDK** — `brew install OleksandrGeronime/serp/serp-dev` (macOS) or `.deb` from [releases](https://github.com/OleksandrGeronime/serp/releases)
- **serpgen** — included in the SDK (`serpgen --version`)
- CMake 3.22+, C++17 compiler
- DBus deployment: `sdbus-c++`, `systemd`
- QtQuick deployment: Qt6

## Generate

```bash
scripts/generate.sh
```

Or directly from the repo root (before scripts are generated):

```bash
serpgen generate-workspace -w specs/serp.sidl
```

## Build

```bash
scripts/build.sh --deployment multiprocess-dbus -j4
scripts/build.sh -d multiprocess-dbus-qtquick -jx
```

Options:

```text
-d, --deployment NAME    select variant
-jN                      parallel jobs
-jx                      use all CPU cores
-c                       clean variant first
--help                   list available variants
```

Build outputs: `build/variants/`

## Run

```bash
scripts/run.sh --deployment multiprocess-dbus
scripts/stop.sh
```

## HMI Frontends

Both frontends connect over the generated `IHmiGateway` transport boundary. The deployment spec selects which frontend is wired in.

**Console** (terminal):

```text
Mouse / 1-8     select entries and soft keys
H/L or arrows   switch apps
D/C             Driver / CoDriver context
R               refresh
Q               quit
```

Screens: Home · Media · Radio · Navigation · Phone · Climate · Settings · Profile · Vehicle · Notifications

**QtQuick** — full graphical HMI, same screens over the same gateway interface.

## License

This project (Minivi demo code) is released under the [MIT License](LICENSE).

The underlying [SERP](https://github.com/OleksandrGeronime/serp) framework is distributed
in binary form under separate terms: free for non-commercial use, commercial use requires
a purchased license. See the SERP repository for details.
