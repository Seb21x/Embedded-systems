#pragma once
#include "mbed.h"
// Start with 2 (B), then 3, 4, 5, 6 and finally 9. G is included from stage 3.
#define APP_STAGE 2
namespace cfg {
constexpr PinName TEMP_PIN=A0, LIGHT_PIN=A1, ROTARY_PIN=A2;
constexpr PinName EDIT_PIN=D2, ACK_PIN=PC_13;
constexpr PinName ROOM_LED_PIN=D3, BUZZER_PIN=D9;
constexpr PinName SDA_PIN=D14, SCL_PIN=D15, PROBE_PIN=D7;
constexpr int EDIT_ACTIVE=1, ACK_ACTIVE=1;
constexpr bool ENABLE_PROBE=false;
// Supply ADC modules from 3.3 V. Calibrate these with raw values from serial.
constexpr float LIGHT_DARK=0.0f, LIGHT_BRIGHT=1.0f;
constexpr unsigned SAMPLE_MS=100, DISPLAY_MS=250;
}
