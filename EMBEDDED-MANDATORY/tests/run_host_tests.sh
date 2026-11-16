#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
build_dir="$(mktemp -d tests/build.XXXXXXXX)"
trap 'rm -rf "$build_dir"' EXIT
includes=(-Itests/fakes -Iinclude -Ilib/Grove_LCD_RGB_Backlight)
# Fake HAL validates application C++ and logic, NOT ARM/Mbed compatibility.
g++ -std=c++14 -Wall -Wextra -Werror "${includes[@]}" tests/host_tests.cpp src/*.cpp lib/Grove_LCD_RGB_Backlight/*.cpp -o "$build_dir/tests"
"$build_dir/tests"
# Stage 9 parses LCD and all peripherals too; override a temporary config copy.
for stage in 2 3 4 5 6 9; do
    mkdir -p "$build_dir/stage$stage"
    cp include/*.h "$build_dir/stage$stage/"
    sed -i "s/#define APP_STAGE 2/#define APP_STAGE $stage/" "$build_dir/stage$stage/AppConfig.h"
    g++ -std=c++14 -Wall -Wextra -Werror -Itests/fakes -I"$build_dir/stage$stage" -Ilib/Grove_LCD_RGB_Backlight -fsyntax-only main.cpp src/*.cpp lib/Grove_LCD_RGB_Backlight/*.cpp
done
printf 'PASS: host syntax checks for stages 2,3,4,5,6,9\n'
