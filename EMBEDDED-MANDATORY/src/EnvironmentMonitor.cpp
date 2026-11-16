#include "EnvironmentMonitor.h"
#include <cmath>
#include <cstdio>
#include <algorithm>
#include <chrono>
static_assert(APP_STAGE>=2 && APP_STAGE<=9,"APP_STAGE must be 2..9");
static_assert(cfg::LIGHT_BRIGHT>cfg::LIGHT_DARK,"Invalid light calibration");
EnvironmentMonitor::EnvironmentMonitor()
 :_temperature(cfg::TEMP_PIN),_light(cfg::LIGHT_PIN),
 _thresholds(cfg::ROTARY_PIN,cfg::EDIT_PIN),
 _normal(LED1,0),_warning(LED2,0),_alarmLed(LED3,0),
 _probe(cfg::ENABLE_PROBE?cfg::PROBE_PIN:NC,0)
#if APP_STAGE >= 4
 ,_button(cfg::ACK_PIN,cfg::ACK_ACTIVE)
#endif
#if APP_STAGE >= 5
 ,_buzzer(cfg::BUZZER_PIN)
#endif
#if APP_STAGE >= 6
 ,_lighting(cfg::ROOM_LED_PIN,cfg::LIGHT_DARK,cfg::LIGHT_BRIGHT)
#endif
#if APP_STAGE >= 9
 ,_display(cfg::SDA_PIN,cfg::SCL_PIN)
#endif
{}
void EnvironmentMonitor::run(){
    _clock.start();
    uint64_t nextSample=0,nextDisplay=0,nextLog=0;
    float temperature=NAN,rawLight=0,light=0,lightPercent=0,ledPercent=0;
    bool valid=false;
    SystemState state=SystemState::NORMAL;
    while(true){
        const uint64_t now=std::chrono::duration_cast<std::chrono::milliseconds>(_clock.elapsed_time()).count();
        _thresholds.update(now);
#if APP_STAGE >= 3
        bool ack=false;
#endif
#if APP_STAGE >= 4
        ack=_button.poll(now,state==SystemState::ALARM);
#endif
        if(now>=nextSample){
            nextSample=now+cfg::SAMPLE_MS;
            if(cfg::ENABLE_PROBE)_probe=1;
            const float measured=_temperature.getTemperatureCelsius();
            valid=std::isfinite(measured) && measured>=-40 && measured<=125;
            if(valid){
#if APP_STAGE >= 3
                temperature=_tempFilter.update(measured);
#else
                temperature=measured;
#endif
            }
            rawLight=_light.getRaw();
#if APP_STAGE >= 3
            light=_lightFilter.update(rawLight);
#else
            light=rawLight;
#endif
            lightPercent=100*std::max(0.0f,std::min(1.0f,(light-cfg::LIGHT_DARK)/(cfg::LIGHT_BRIGHT-cfg::LIGHT_DARK)));
#if APP_STAGE >= 6
            _lighting.update(light);ledPercent=_lighting.duty()*100;
#endif
            if(cfg::ENABLE_PROBE)_probe=0;
        }
#if APP_STAGE >= 3
        state=_alarm.update(temperature,_thresholds.getLowThreshold(),_thresholds.getHighThreshold(),valid,ack);
#endif
        _normal=state==SystemState::NORMAL;
        _warning=state==SystemState::WARNING || state==SystemState::ACKNOWLEDGED;
        _alarmLed=state==SystemState::ALARM || state==SystemState::ACKNOWLEDGED;
#if APP_STAGE >= 5
        _buzzer.update(state,now);
#endif
        if(now>=nextDisplay){
            nextDisplay=now+cfg::DISPLAY_MS;
#if APP_STAGE >= 9
            const DisplayData data={temperature,lightPercent,ledPercent,
                _thresholds.getLowThreshold(),_thresholds.getHighThreshold(),_thresholds.getKnobTemperature(),
                valid,_thresholds.isEditingHigh(),_thresholds.isCaptured(),state};
            _display.update(data,now);
#endif
        }
        if(now>=nextLog){
            nextLog=now+500;
            printf("ms=%llu T=%.2f valid=%d L=%.1f H=%.1f edit=%s %s knob=%.1f rawLight=%.4f light=%.1f%% LED=%.1f%% state=%s\n",
              (unsigned long long)now,temperature,int(valid),_thresholds.getLowThreshold(),_thresholds.getHighThreshold(),
              _thresholds.isEditingHigh()?"HIGH":"LOW",_thresholds.isCaptured()?"SET":"PICK",
              _thresholds.getKnobTemperature(),rawLight,lightPercent,ledPercent,AlarmController::name(state));
        }
        ThisThread::sleep_for(std::chrono::milliseconds(10));
    }
}
