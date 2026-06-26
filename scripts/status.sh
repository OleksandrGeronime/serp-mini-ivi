#!/usr/bin/env bash
# Show the live status of all generated processes in a product workspace deployment.
# Usage: status.sh --deployment NAME
set -euo pipefail
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
source "$SCRIPT_DIR/lib/common.sh"

usage() {
  cat <<USAGE
Usage:
  $0 --deployment NAME
  $0 -d NAME

Required:
  -d, --deployment NAME    Available: monolith multiprocess_dbus_console multiprocess_dbus_qtquick multiprocess_grpc_qtquick
USAGE
}

deployment=""
while [[ $# -gt 0 ]]; do
  case "$1" in
    -d|--deployment) deployment="${2:-}"; shift 2 ;;
    -h|--help) usage; exit 0 ;;
    *) echo "Unknown option: $1" >&2; usage; exit 2 ;;
  esac
done

resolved=$(resolve_deployment "$deployment") || { echo "Invalid or unknown deployment: $deployment" >&2; usage; exit 2; }
build_dir=$(variant_dir "$resolved")

declare -a PROC_KEYS=()
declare -A PROC_NAME=()
declare -A PROC_EXE=()
declare -A PROC_DBUS=()   # space-separated list of full DBus well-known names

case "$resolved" in
  'monolith')
    PROC_KEYS=()
    PROC_KEYS+=(p0)
    PROC_NAME[p0]="minivi_app"
    PROC_EXE[p0]="${build_dir}/monolith/monolith_minivi_app"
    PROC_DBUS[p0]=""
    ;;
  'multiprocess_dbus_console')
    PROC_KEYS=()
    PROC_KEYS+=(p0)
    PROC_NAME[p0]="vehicle_hal_app"
    PROC_EXE[p0]="${build_dir}/multiprocess_dbus_console/multiprocess_dbus_console_vehicle_hal_app"
    PROC_DBUS[p0]="serp.generated.vehiclehal serp.generated.climatehal serp.generated.btgateway serp.generated.radiohal serp.generated.locationhal serp.generated.persistencehal"
    PROC_KEYS+=(p1)
    PROC_NAME[p1]="audio_hal_app"
    PROC_EXE[p1]="${build_dir}/multiprocess_dbus_console/multiprocess_dbus_console_audio_hal_app"
    PROC_DBUS[p1]="serp.generated.audiohal"
    PROC_KEYS+=(p2)
    PROC_NAME[p2]="car_service_app"
    PROC_EXE[p2]="${build_dir}/multiprocess_dbus_console/multiprocess_dbus_console_car_service_app"
    PROC_DBUS[p2]="serp.generated.persistenceservice serp.generated.carnotificationmanager serp.generated.carpropertyservice serp.generated.carpowermanager serp.generated.carclimatemanager serp.generated.carnavigationmanager serp.generated.carusermanager"
    PROC_KEYS+=(p3)
    PROC_NAME[p3]="phone_app"
    PROC_EXE[p3]="${build_dir}/multiprocess_dbus_console/multiprocess_dbus_console_phone_app"
    PROC_DBUS[p3]="serp.generated.carcallmanager serp.generated.carcontactsmanager"
    PROC_KEYS+=(p4)
    PROC_NAME[p4]="media_app"
    PROC_EXE[p4]="${build_dir}/multiprocess_dbus_console/multiprocess_dbus_console_media_app"
    PROC_DBUS[p4]="serp.generated.caraudiomanager serp.generated.carmediamanager serp.generated.carradiomanager"
    ;;
  'multiprocess_dbus_qtquick')
    PROC_KEYS=()
    PROC_KEYS+=(p0)
    PROC_NAME[p0]="vehicle_hal_app"
    PROC_EXE[p0]="${build_dir}/multiprocess_dbus_qtquick/multiprocess_dbus_qtquick_vehicle_hal_app"
    PROC_DBUS[p0]="serp.generated.vehiclehal serp.generated.climatehal serp.generated.btgateway serp.generated.radiohal serp.generated.locationhal serp.generated.persistencehal"
    PROC_KEYS+=(p1)
    PROC_NAME[p1]="audio_hal_app"
    PROC_EXE[p1]="${build_dir}/multiprocess_dbus_qtquick/multiprocess_dbus_qtquick_audio_hal_app"
    PROC_DBUS[p1]="serp.generated.audiohal"
    PROC_KEYS+=(p2)
    PROC_NAME[p2]="car_service_app"
    PROC_EXE[p2]="${build_dir}/multiprocess_dbus_qtquick/multiprocess_dbus_qtquick_car_service_app"
    PROC_DBUS[p2]="serp.generated.persistenceservice serp.generated.carnotificationmanager serp.generated.carpropertyservice serp.generated.carpowermanager serp.generated.carclimatemanager serp.generated.carnavigationmanager serp.generated.carusermanager"
    PROC_KEYS+=(p3)
    PROC_NAME[p3]="phone_app"
    PROC_EXE[p3]="${build_dir}/multiprocess_dbus_qtquick/multiprocess_dbus_qtquick_phone_app"
    PROC_DBUS[p3]="serp.generated.carcallmanager serp.generated.carcontactsmanager"
    PROC_KEYS+=(p4)
    PROC_NAME[p4]="media_app"
    PROC_EXE[p4]="${build_dir}/multiprocess_dbus_qtquick/multiprocess_dbus_qtquick_media_app"
    PROC_DBUS[p4]="serp.generated.caraudiomanager serp.generated.carmediamanager serp.generated.carradiomanager"
    ;;
  'multiprocess_grpc_qtquick')
    PROC_KEYS=()
    PROC_KEYS+=(p0)
    PROC_NAME[p0]="vehicle_hal_app"
    PROC_EXE[p0]="${build_dir}/multiprocess_grpc_qtquick/multiprocess_grpc_qtquick_vehicle_hal_app"
    PROC_DBUS[p0]=""
    PROC_KEYS+=(p1)
    PROC_NAME[p1]="audio_hal_app"
    PROC_EXE[p1]="${build_dir}/multiprocess_grpc_qtquick/multiprocess_grpc_qtquick_audio_hal_app"
    PROC_DBUS[p1]=""
    PROC_KEYS+=(p2)
    PROC_NAME[p2]="car_service_app"
    PROC_EXE[p2]="${build_dir}/multiprocess_grpc_qtquick/multiprocess_grpc_qtquick_car_service_app"
    PROC_DBUS[p2]=""
    PROC_KEYS+=(p3)
    PROC_NAME[p3]="phone_app"
    PROC_EXE[p3]="${build_dir}/multiprocess_grpc_qtquick/multiprocess_grpc_qtquick_phone_app"
    PROC_DBUS[p3]=""
    PROC_KEYS+=(p4)
    PROC_NAME[p4]="media_app"
    PROC_EXE[p4]="${build_dir}/multiprocess_grpc_qtquick/multiprocess_grpc_qtquick_media_app"
    PROC_DBUS[p4]=""
    ;;
  *) echo "Invalid or unknown deployment: $resolved" >&2; exit 2 ;;
esac

proc_pid() {
  local executable="$1"
  pgrep -f -- "$executable" 2>/dev/null | head -1 || true
}

proc_dbus_address() {
  local pid="$1"
  if [[ -z "$pid" || "$pid" == "-" || ! -r "/proc/$pid/environ" ]]; then
    return 0
  fi
  tr '\0' '\n' < "/proc/$pid/environ" \
    | sed -n 's/^DBUS_SESSION_BUS_ADDRESS=//p' \
    | head -1
}

dbus_registered() {
  local bus_name="$1"
  local bus_address="${2:-}"
  if ! command -v gdbus >/dev/null 2>&1; then echo "?"; return; fi
  if [[ -n "$bus_address" ]]; then
    if DBUS_SESSION_BUS_ADDRESS="$bus_address" gdbus call --session \
         --dest org.freedesktop.DBus \
         --object-path /org/freedesktop/DBus \
         --method org.freedesktop.DBus.GetNameOwner \
         "$bus_name" >/dev/null 2>&1; then
      echo "yes"
    else
      echo "no"
    fi
  elif gdbus call --session \
       --dest org.freedesktop.DBus \
       --object-path /org/freedesktop/DBus \
       --method org.freedesktop.DBus.GetNameOwner \
       "$bus_name" >/dev/null 2>&1; then
    echo "yes"
  else
    echo "no"
  fi
}

printf "\n  SERP workspace status — deployment: %s\n" "$resolved"
printf "  DBus bus: process session bus when available\n\n"
printf "  %-8s %-22s %-10s %-10s %-10s  %s\n" "PID" "PROCESS" "CPU" "MEM" "STATE" "DBUS SERVICES"
printf "  %-8s %-22s %-10s %-10s %-10s  %s\n" "---" "-------" "---" "---" "-----" "-------------"

for key in "${PROC_KEYS[@]}"; do
  proc="${PROC_NAME[$key]}"
  exe="${PROC_EXE[$key]}"
  dbus_names="${PROC_DBUS[$key]:-}"

  pid=$(proc_pid "$exe")
  if [[ -n "$pid" ]]; then
    state="running"
    read -r cpu mem < <(ps -p "$pid" -o %cpu=,%mem= 2>/dev/null || echo "0 0")
    cpu=$(printf "%.1f%%" "$cpu" 2>/dev/null || echo "-")
    mem=$(printf "%.1f%%" "$mem" 2>/dev/null || echo "-")
  else
    state="stopped"
    cpu="-"
    mem="-"
    pid="-"
  fi

  dbus_status=""
  if [[ -n "$dbus_names" && "$state" == "running" ]]; then
    dbus_bus=$(proc_dbus_address "$pid" || true)
    for svc in $dbus_names; do
      reg=$(dbus_registered "$svc" "$dbus_bus")
      dbus_status+="${svc}:${reg}  "
    done
  elif [[ -n "$dbus_names" ]]; then
    for svc in $dbus_names; do
      dbus_status+="${svc}:no  "
    done
  fi

  printf "  %-8s %-22s %-10s %-10s %-10s  %s\n" "$pid" "$proc" "$cpu" "$mem" "$state" "${dbus_status:-n/a}"
done

printf "\n"
