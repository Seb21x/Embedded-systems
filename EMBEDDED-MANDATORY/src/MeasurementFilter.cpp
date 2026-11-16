#include "MeasurementFilter.h"
float MeasurementFilter::update(float sample){
    if(!_ready){_value=sample;_ready=true;}else _value+=0.2f*(sample-_value);
    return _value;
}
