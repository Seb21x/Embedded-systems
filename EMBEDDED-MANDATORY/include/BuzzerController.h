#pragma once
#include "AlarmController.h"
#include "mbed.h"
class BuzzerController
{
public:
    explicit BuzzerController(PinName pin);
    void update(SystemState state, uint64_t nowMs);
    static bool soundOn(SystemState state, uint64_t elapsedMs);

private:
    PwmOut _buzzer;
    SystemState _previous = SystemState::NORMAL;
    uint64_t _started = 0;
};
