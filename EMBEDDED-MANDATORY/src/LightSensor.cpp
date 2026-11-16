#include "LightSensor.h"
LightSensor::LightSensor(PinName pin):_pin(pin){}
float LightSensor::getRaw(){return _pin.read();}
