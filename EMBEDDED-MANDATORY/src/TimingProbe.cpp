#include "TimingProbe.h"

TimingProbe::TimingProbe(PinName pin) : _pin(pin, 0)
{
}

void TimingProbe::beginSample()
{
    _pin = 1;
}

void TimingProbe::endSample()
{
    _pin = 0;
}
