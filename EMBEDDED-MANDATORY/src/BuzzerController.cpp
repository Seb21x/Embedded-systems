#include "BuzzerController.h"
BuzzerController::BuzzerController(PinName pin):_buzzer(pin){_buzzer.period_us(500);_buzzer.write(0);}
bool BuzzerController::soundOn(SystemState s,uint64_t t){
    return s==SystemState::WARNING?t%1000<100:s==SystemState::ALARM && t%300<150;
}
void BuzzerController::update(SystemState s,uint64_t now){
    if(s!=_previous){_previous=s;_started=now;}
    _buzzer.write(soundOn(s,now-_started)?0.5f:0.0f);
}
