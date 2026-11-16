#pragma once
class MeasurementFilter {
public:
    float update(float sample);
private:
    bool _ready=false;
    float _value=0;
};
