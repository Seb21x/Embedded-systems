#include "AlarmButton.h"
AlarmButton::AlarmButton(PinName pin,int active):_button(pin),_active(active){
    if(active)_button.rise(callback(this,&AlarmButton::onPress));
    else _button.fall(callback(this,&AlarmButton::onPress));
}
void AlarmButton::onPress(){_edge=true;}
bool AlarmButton::poll(uint64_t now,bool alarm){
    core_util_critical_section_enter();const bool edge=_edge;_edge=false;core_util_critical_section_exit();
    const bool down=_button.read()==_active;
    if(_waitingRelease){
        if(down)_releaseTiming=false;
        else if(!_releaseTiming){_releaseTiming=true;_releasedAt=now;}
        else if(now-_releasedAt>=50){_waitingRelease=false;_releaseTiming=false;}
        return false;
    }
    if(edge && !_pending){
        if(!alarm){_waitingRelease=true;return false;}
        _pending=true;_pressedAt=now;
    }
    if(_pending){
        if(!alarm || !down){_pending=false;return false;}
        if(now-_pressedAt>=50){_pending=false;_waitingRelease=true;return true;}
    }
    return false;
}
