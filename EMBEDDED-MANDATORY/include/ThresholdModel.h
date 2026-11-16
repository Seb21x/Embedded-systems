#pragma once
class ThresholdModel {
public:
    void update(float raw,bool selectHigh,float minimum=10,float maximum=40);
    float low=18, high=28, knob=0;
    bool highSelected=false, captured=false;
private:
    bool havePrevious=false;
    float previous=0;
};
