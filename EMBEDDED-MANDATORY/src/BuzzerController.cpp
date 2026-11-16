#include "BuzzerController.h"
BuzzerController::BuzzerController(PinName pin) : _buzzer(pin)
{
    _buzzer.period_us(500);
    _buzzer.write(0);
}
bool BuzzerController::soundOn(SystemState state, uint64_t elapsedMs)
{
    return state == SystemState::WARNING ? elapsedMs % 1000 < 100
                                         : state == SystemState::ALARM && elapsedMs % 300 < 150;
}
void BuzzerController::update(SystemState state, uint64_t nowMs)
{
    if (state != _previous)
    {
        _previous = state;
        _started = nowMs;
    }
    _buzzer.write(soundOn(state, nowMs - _started) ? 0.5f : 0.0f);
}
