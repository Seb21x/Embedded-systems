#ifndef MBED_TEMPSENSOR_H
#define MBED_TEMPSENSOR_H

#include "mbed.h"

class TempSensor
{
public:
    TempSensor(PinName pin, float beta = 4250.0f);
    float getTemperatureKelvin();
    float getTemperatureCelsius();

private:
    AnalogIn _pin;
    float _beta;

    static constexpr float R0 = 100000.0f;
    static constexpr float RB = 100000.0f;
    static constexpr float T0 = 298.15f;
    static constexpr float K_OFFSET = 273.15f;
    static constexpr float ADC_RES = 1.0f; // read()
};

#endif