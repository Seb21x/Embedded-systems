# Walidacja 2026-11-19

Baza Git: 78d8bb7. Testy: `bash tests/run_host_tests.sh`.

- PASS: alarm, thresholds, PWM, ADC, button, filter/reset, calendar rollover, probe, LCD pages/cache.
- PASS: full application host syntax (not ARM/Mbed build).

Kompilator g++, C++14, -Wall -Wextra -Werror. Użyto atrap Mbed HAL.
Nie uruchomiono kompilatora ARM/Mbed ani płytki. Brak pomiarów elektrycznych H.
Tabela POMIARY_H.csv ma puste wyniki do uzupełnienia na stanowisku.
