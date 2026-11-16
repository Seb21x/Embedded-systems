#include "AlarmButton.h"
#include "AlarmController.h"
#include "BuzzerController.h"
#include "LightSensor.h"
#include "LightingController.h"
#include "TempSensor.h"
#include "ThresholdController.h"
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
    Timer timer;
    timer.start();
    uint64_t nextSample = 0, nextPrint = 0;
    float temperatureC = NAN, lightPercent = 0;
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
            thresholds.update();
            temperatureC = temperature.getTemperatureCelsius();
            valid = std::isfinite(temperatureC) && temperatureC >= -40 && temperatureC <= 125;
            lightPercent = light.getLightPercentage();
            lamp.update(lightPercent);
        }
        const SystemState state =
            alarm.update(temperatureC, thresholds.getLowThreshold(), thresholds.getHighThreshold(),
                         valid, acknowledgeRequested);
        buzzer.update(state, nowMs);
        green = state == SystemState::NORMAL;
        blue = state == SystemState::WARNING || state == SystemState::ACKNOWLEDGED;
        red = state == SystemState::ALARM || state == SystemState::ACKNOWLEDGED;
        if (nowMs >= nextPrint)
        {
            nextPrint = nowMs + 500;
            printf("T=%.2f valid=%d LOW=%.1f HIGH=%.1f EDIT=%s Light=%.1f%% LED=%.1f%% State=%s\n",
                   temperatureC, int(valid), thresholds.getLowThreshold(),
                   thresholds.getHighThreshold(), thresholds.isEditingHigh() ? "HIGH" : "LOW",
                   lightPercent, 100 - lightPercent, AlarmController::name(state));
        }
        ThisThread::sleep_for(std::chrono::milliseconds(10));
    }
}
