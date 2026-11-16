#pragma once
#include "mbed.h"
class LightSensor {
public:
    explicit LightSensor(PinName pin);
    float getRaw();
private:
    AnalogIn _pin;
};
