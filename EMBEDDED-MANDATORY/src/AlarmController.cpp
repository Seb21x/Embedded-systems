#include "AlarmController.h"
SystemState AlarmController::update(float t,float low,float high,bool valid,bool ack){
        if(!valid){ // Sensor fault stays visible; button may silence it.
            const bool wasAlarm=_state==SystemState::ALARM;
            if(_state!=SystemState::ACKNOWLEDGED)_state=SystemState::ALARM;
            if(ack && wasAlarm)_state=SystemState::ACKNOWLEDGED;
            return _state;
        }
        const bool safe=t>=low+0.5f && t<=high-0.5f;
        if(_state==SystemState::ACKNOWLEDGED){if(safe)_state=SystemState::NORMAL;return _state;}
        const bool wasAlarm=_state==SystemState::ALARM;
        if(t<=low-3 || t>=high+3)_state=SystemState::ALARM;
        else if(_state==SystemState::ALARM){
            if(safe)_state=SystemState::NORMAL;
            else if(t>=low-2.5f && t<=high+2.5f)_state=SystemState::WARNING;
        }else if(_state==SystemState::WARNING){if(safe)_state=SystemState::NORMAL;}
        else if(t<low || t>high)_state=SystemState::WARNING;
        // A click from NORMAL/WARNING never acknowledges a newly entered alarm.
        if(ack && wasAlarm && _state==SystemState::ALARM)_state=SystemState::ACKNOWLEDGED;
        return _state;
    }
SystemState AlarmController::state()const{return _state;}
const char* AlarmController::name(SystemState s){
 switch(s){case SystemState::NORMAL:return "NORMAL";case SystemState::WARNING:return "WARNING";case SystemState::ALARM:return "ALARM";default:return "ACK";}
}
