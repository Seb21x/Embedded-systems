#include "mbed.h"
#include "TempSensor.h"
#include "LightSensor.h"
#include "ThresholdController.h"

TempSensor tempSensor(A0);
LightSensor lightSensor(A1);
ThresholdController thresholds(A2, D2, 10.0f, 40.0f);

int main() {
    while (true) {
        thresholds.update();

        float temperatureC = tempSensor.getTemperatureCelsius();
        float illuminanceLux = lightSensor.getLux();
        float lightPercent = lightSensor.getLightPercentage();

        float lowThresh = thresholds.getLowThreshold();
        float highThresh = thresholds.getHighThreshold();
        const char* activeEdit = thresholds.isEditingHigh() ? "HIGH" : "LOW";

        printf("Temp: %.2f C [Low: %.1f C | High: %.1f C | Edit: %s] | Light: %.1f %%\n",
               temperatureC, lowThresh, highThresh, activeEdit, lightPercent);

        ThisThread::sleep_for(600ms);
    }
}