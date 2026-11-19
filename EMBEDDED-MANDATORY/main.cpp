#include "AlarmButton.h"
#include "AlarmController.h"
#include "BuzzerController.h"
#include "CalendarClock.h"
#include "DisplayController.h"
#include "LightSensor.h"
#include "LightingController.h"
#include "MeasurementFilter.h"
#include "TempSensor.h"
#include "ThresholdController.h"
#include "TimingProbe.h"
#include "mbed.h"
#include <chrono>
#include <cmath>

int main()
{
    TempSensor temperature(A0);
    LightSensor light(A1);
    ThresholdController thresholds(A2, D2, 10.0f, 40.0f);
    AlarmController alarm;
    AlarmButton acknowledge(PC_13, 1); // blue USER button, active HIGH
    BuzzerController buzzer(D5);       // Grove socket D5, 2 kHz
    LightingController lamp(D3);       // Grove socket D3, SAME 2 kHz (TIM1)
    DigitalOut green(LED1), blue(LED2), red(LED3);
    MeasurementFilter temperatureFilter;
    MeasurementFilter lightFilter;
    TimingProbe probe(D7);
    DisplayController display(D14, D15);
    // Approximate calendar start: rebuild before demonstration.
    // It is NOT synchronized and resets to this date/time on restart.
    CalendarClock clock(__DATE__, __TIME__);
    Timer timer;
    timer.start();
    uint64_t nextSample = 0, nextPrint = 0, nextDisplay = 0;
    float temperatureC = NAN, lightPercent = 0;
    float rawTemperature = NAN, rawLight = 0;
    bool valid = false;
    while (true)
    {
        const uint64_t nowMs =
            std::chrono::duration_cast<std::chrono::milliseconds>(timer.elapsed_time()).count();
        // Consume button events before detecting a new alarm.
        const bool acknowledgeRequested =
            acknowledge.poll(nowMs, alarm.state() == SystemState::ALARM);
        if (nowMs >= nextSample)
        {
            nextSample = nowMs + 100;
            probe.beginSample();
            thresholds.update();
            rawTemperature = temperature.getTemperatureCelsius();
            valid = std::isfinite(rawTemperature) && rawTemperature >= -40 && rawTemperature <= 125;
            if (valid)
            {
                temperatureC = temperatureFilter.update(rawTemperature);
            }
            else
            {
                temperatureFilter.reset();
                temperatureC = NAN;
            }
            rawLight = light.getLightPercentage();
            lightPercent = lightFilter.update(rawLight);
            lamp.update(lightPercent);
            probe.endSample();
        }
        const SystemState state =
            alarm.update(temperatureC, thresholds.getLowThreshold(), thresholds.getHighThreshold(),
                         valid, acknowledgeRequested);
        buzzer.update(state, nowMs);
        green = state == SystemState::NORMAL;
        blue = state == SystemState::WARNING || state == SystemState::ACKNOWLEDGED;
        red = state == SystemState::ALARM || state == SystemState::ACKNOWLEDGED;
        if (nowMs >= nextDisplay)
        {
            nextDisplay = nowMs + 250;
            const DisplayData data = {temperatureC,
                                      lightPercent,
                                      thresholds.getLowThreshold(),
                                      thresholds.getHighThreshold(),
                                      valid,
                                      thresholds.isEditingHigh(),
                                      state};
            display.update(data, clock, nowMs);
        }
        if (nowMs >= nextPrint)
        {
            nextPrint = nowMs + 500;
            printf("Traw=%.2f Lraw=%.1f T=%.2f valid=%d LOW=%.1f HIGH=%.1f EDIT=%s Light=%.1f%% "
                   "LED=%.1f%% State=%s\n",
                   rawTemperature, rawLight, temperatureC, int(valid), thresholds.getLowThreshold(),
                   thresholds.getHighThreshold(), thresholds.isEditingHigh() ? "HIGH" : "LOW",
                   lightPercent, 100 - lightPercent, AlarmController::name(state));
        }
        ThisThread::sleep_for(std::chrono::milliseconds(10));
    }
}
