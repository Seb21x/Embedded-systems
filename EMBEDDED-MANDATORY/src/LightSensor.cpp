#include "LightSensor.h"

LightSensor::LightSensor(PinName pin, float vRef) : _pin(pin), _vRef(vRef) {}

float LightSensor::getLux() {
    float normalized = _pin.read() / ADC_RES;
    float vOut = normalized * _vRef;
    float lux = (((_vRef * LUX_REL) * vOut) - LUX_REL) / RL;
    return (lux < 0.0f) ? 0.0f : lux;
}

float LightSensor::getLightPercentage() {
    return (_pin.read() / ADC_RES) * 100.0f;
}