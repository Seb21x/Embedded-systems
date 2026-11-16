#ifndef MBED_LIGHTSENSOR_H
#define MBED_LIGHTSENSOR_H

#include "mbed.h"

class LightSensor
{
public:
    LightSensor(PinName pin, float vRef = 3.3f);
    float getLux();
    float getLightPercentage();

private:
    AnalogIn _pin;
    float _vRef;

    static constexpr float RL = 10.0f;
    static constexpr float LUX_REL = 500.0f;
    static constexpr float ADC_RES = 1.0f; // read()
};

#endif