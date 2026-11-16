#include "LightingController.h"
#include <algorithm>
LightingController::LightingController(PinName pin) : _led(pin)
{
    // D3 and D5 share TIM1: BOTH outputs must use the SAME period.
    _led.period_us(500);
    _led.write(0);
}
void LightingController::update(float lightPercent)
{
    const float duty = 1.0f - std::max(0.0f, std::min(100.0f, lightPercent)) / 100.0f;
    _led.write(duty);
}
