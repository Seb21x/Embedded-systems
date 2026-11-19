#pragma once
#include "mbed.h"

// D7 is reserved for optional oscilloscope/logic-analyzer measurements.
class TimingProbe
{
public:
    explicit TimingProbe(PinName pin);
    void beginSample();
    void endSample();

private:
    DigitalOut _pin;
};
