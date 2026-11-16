#pragma once
#include "mbed.h"
#include <cstdint>
class AlarmButton
{
public:
    AlarmButton(PinName pin, int activeLevel);
    bool poll(uint64_t nowMs, bool alarmActive);

private:
    void onPress();
    InterruptIn _button;
    const int _active;
    volatile bool _edge = false;
    bool _pending = false, _waitingRelease = false, _releaseTiming = false;
    uint64_t _pressedAt = 0, _releasedAt = 0;
};
