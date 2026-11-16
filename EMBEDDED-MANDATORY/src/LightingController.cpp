#include "LightingController.h"
#include <algorithm>
LightingController::LightingController(PinName pin,float dark,float bright)
 :_led(pin),_dark(dark),_bright(bright){_led.period_us(1000);_led.write(0);}
float LightingController::calculatePercentage(float raw,float dark,float bright){
    if(!(bright>dark))return 0;
    return 100*std::max(0.0f,std::min(1.0f,(raw-dark)/(bright-dark)));
}
void LightingController::update(float raw){
    _percent=calculatePercentage(raw,_dark,_bright);_duty=1-_percent/100;_led.write(_duty);
}
float LightingController::percentage()const{return _percent;}
float LightingController::duty()const{return _duty;}
