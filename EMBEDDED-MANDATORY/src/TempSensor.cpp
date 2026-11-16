#include "TempSensor.h"
#include <cmath>

TempSensor::TempSensor(PinName pin, float beta) : _pin(pin), _beta(beta) {}

float TempSensor::getTemperatureKelvin() {
    float counts = _pin.read();
    if (!std::isfinite(counts) || counts <= 0.0001f || counts >= 0.9999f) {
        return NAN;
    }

    float rTherm = RB * ((ADC_RES / counts) - 1.0f);
    float tempK = 1.0f / ((std::log(rTherm / R0) / _beta) + (1.0f / T0));
    return tempK;
}

float TempSensor::getTemperatureCelsius() {
    return getTemperatureKelvin() - K_OFFSET;
}