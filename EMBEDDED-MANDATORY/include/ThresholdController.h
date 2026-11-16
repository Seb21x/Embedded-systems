#pragma once
#include "mbed.h"
#include "ThresholdModel.h"
#include "MeasurementFilter.h"
class ThresholdController {
public:
    ThresholdController(PinName rotaryPin,PinName buttonPin,float minTemp=10,float maxTemp=40);
    void update(uint64_t nowMs);
    float getLowThreshold()const{return _values.low;}
    float getHighThreshold()const{return _values.high;}
    bool isEditingHigh()const{return _values.highSelected;}
    bool isCaptured()const{return _values.captured;}
    float getKnobTemperature()const{return _values.knob;}
private:
    AnalogIn _rotary; DigitalIn _button;
    float _minTemp,_maxTemp;
    bool _candidate=false,_stable=false;
    uint64_t _changedAt=0;
    ThresholdModel _values;
    MeasurementFilter _filter;
};
