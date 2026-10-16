#!/bin/bash
# Renders every dashboard page to PNG on a Linux PC (see README → Screenshots).
# Usage: build.sh <repo dir> <lvgl 8.3 dir> <out dir>
set -e
REPO=$1; LVGL=$2; OUT=$3; HERE=$(cd "$(dirname "$0")" && pwd)
mkdir -p "$OUT" /tmp/p3obj
FLAGS="-O1 -w -DLV_CONF_INCLUDE_SIMPLE -I$HERE/stubs -I$REPO/src -I$LVGL/.. -I$LVGL"
objs=()
for f in $(find "$LVGL/src" -name '*.c') $(find "$REPO/src/fonts" -name '*.c'); do
  o=/tmp/p3obj/$(echo "$f" | md5sum | cut -c1-12).o
  [ -f "$o" ] || gcc $FLAGS -c "$f" -o "$o"; objs+=("$o")
done
cpp=$(find "$REPO/src/display" -name '*.cpp' ! -name epd_driver.cpp)
g++ -std=gnu++17 $FLAGS $cpp "$HERE/stubs.cpp" "$HERE/render.cpp" "${objs[@]}" -lz -o /tmp/p3obj/render
/tmp/p3obj/render "$OUT"
