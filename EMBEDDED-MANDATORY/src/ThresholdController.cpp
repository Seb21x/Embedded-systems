#include "ThresholdController.h"
#include "AppConfig.h"
ThresholdController::ThresholdController(PinName r,PinName b,float lo,float hi)
    :_rotary(r),_button(b,cfg::EDIT_ACTIVE?PullDown:PullUp),_minTemp(lo),_maxTemp(hi){}
void ThresholdController::update(uint64_t now){
    const bool pressed=_button.read()==cfg::EDIT_ACTIVE;
    if(pressed!=_candidate){_candidate=pressed;_changedAt=now;}
    if(now-_changedAt>=50)_stable=_candidate;
    _values.update(_filter.update(_rotary.read()),_stable,_minTemp,_maxTemp);
}
