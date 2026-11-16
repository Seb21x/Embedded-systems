#pragma once
#include "AppConfig.h"
#include "TempSensor.h"
#include "LightSensor.h"
#include "ThresholdController.h"
#include "MeasurementFilter.h"
#include "AlarmController.h"
#if APP_STAGE >= 4
#include "AlarmButton.h"
#endif
#if APP_STAGE >= 5
#include "BuzzerController.h"
#endif
#if APP_STAGE >= 6
#include "LightingController.h"
#endif
#if APP_STAGE >= 9
#include "DisplayController.h"
#endif
class EnvironmentMonitor {
public:
    EnvironmentMonitor();
    void run();
private:
    TempSensor _temperature;
    LightSensor _light;
    ThresholdController _thresholds;
    MeasurementFilter _tempFilter,_lightFilter;
    AlarmController _alarm;
    Timer _clock;
    DigitalOut _normal,_warning,_alarmLed,_probe;
#if APP_STAGE >= 4
    AlarmButton _button;
#endif
#if APP_STAGE >= 5
    BuzzerController _buzzer;
#endif
#if APP_STAGE >= 6
    LightingController _lighting;
#endif
#if APP_STAGE >= 9
    DisplayController _display;
#endif
};
