#pragma once
#include "mbed.h"
class LightingController {
public:
    LightingController(PinName pin,float dark,float bright);
    void update(float filteredRaw);
    float percentage()const;
    float duty()const;
    static float calculatePercentage(float raw,float dark,float bright);
private:
    PwmOut _led;
    float _dark,_bright,_percent=0,_duty=0;
};
