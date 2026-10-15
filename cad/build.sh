#!/usr/bin/env bash
# Export every printable part to stl/ . Usage: ./build.sh [sg90|micro2g]
set -euo pipefail
cd "$(dirname "$0")"
SERVO="${1:-sg90}"
mkdir -p ../stl
for p in gear_drive gear_idler wing_left wing_right case_base case_lid; do
  echo "== $p ($SERVO)"
  openscad -q -o "../stl/${p}.stl" -D "part=\"$p\"" -D "servo_model=\"$SERVO\"" butterfly.scad
done
