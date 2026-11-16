#pragma once
#include "mbed.h"
class LightingController
{
public:
    explicit LightingController(PinName pin);
    void update(float lightPercent);

private:
    PwmOut _led;
};
