#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
build_dir="$(mktemp -d tests/build.XXXXXXXX)"
trap 'rm -rf "$build_dir"' EXIT
includes=(-Itests/fakes -Iinclude -Ilib/Grove_LCD_RGB_Backlight)
g++ -std=c++14 -Wall -Wextra -Werror "${includes[@]}" tests/host_tests.cpp src/*.cpp lib/Grove_LCD_RGB_Backlight/*.cpp -o "$build_dir/tests"
"$build_dir/tests"
g++ -std=c++14 -Wall -Wextra -Werror "${includes[@]}" -fsyntax-only main.cpp src/*.cpp lib/Grove_LCD_RGB_Backlight/*.cpp
printf 'PASS: full application host syntax (not ARM/Mbed build)\n'
