#ifndef MBED_THRESHOLDCONTROLLER_H
#define MBED_THRESHOLDCONTROLLER_H

#include "mbed.h"

class ThresholdController {
public:
    ThresholdController(PinName rotaryPin, PinName buttonPin, float minTemp = 10.0f, 
    float maxTemp = 40.0f);
    void update();
    float getLowThreshold() const;
    float getHighThreshold() const;
    bool isEditingHigh() const;

private:
    AnalogIn _rotary;
    DigitalIn _button;

    float _minTemp;
    float _maxTemp;
    float _lowThreshold;
    float _highThreshold;
    bool _isEditingHigh;
};

#endif