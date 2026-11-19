#include "MeasurementFilter.h"
#include <cmath>

float MeasurementFilter::update(float sample)
{
    if (!std::isfinite(sample))
    {
        reset();
        return sample;
    }
    if (!_ready)
    {
        _value = sample;
        _ready = true;
    }
    else
    {
        _value += 0.2f * (sample - _value);
    }
    return _value;
}

void MeasurementFilter::reset()
{
    _ready = false;
}
