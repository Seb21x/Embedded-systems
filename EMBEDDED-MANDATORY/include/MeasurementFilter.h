#pragma once

class MeasurementFilter
{
public:
    float update(float sample);
    void reset();

private:
    bool _ready = false;
    float _value = 0.0f;
};
