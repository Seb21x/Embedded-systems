#include "AlarmController.h"
SystemState AlarmController::update(float temperature, float lowThreshold, float highThreshold,
                                    bool measurementValid, bool acknowledge)
{
    if (!measurementValid)
    { // Sensor fault stays visible; button may silence it.
        const bool wasAlarm = _state == SystemState::ALARM;
        if (_state != SystemState::ACKNOWLEDGED)
        {
            _state = SystemState::ALARM;
        }
        if (acknowledge && wasAlarm)
        {
            _state = SystemState::ACKNOWLEDGED;
        }
        return _state;
    }
    const bool safe = temperature >= lowThreshold + 0.5f && temperature <= highThreshold - 0.5f;
    if (_state == SystemState::ACKNOWLEDGED)
    {
        if (safe)
        {
            _state = SystemState::NORMAL;
        }
        return _state;
    }
    const bool wasAlarm = _state == SystemState::ALARM;
    if (temperature <= lowThreshold - 3 || temperature >= highThreshold + 3)
    {
        _state = SystemState::ALARM;
    }
    else if (_state == SystemState::ALARM)
    {
        if (safe)
        {
            _state = SystemState::NORMAL;
        }
        else if (temperature >= lowThreshold - 2.5f && temperature <= highThreshold + 2.5f)
        {
            _state = SystemState::WARNING;
        }
    }
    else if (_state == SystemState::WARNING)
    {
        if (safe)
        {
            _state = SystemState::NORMAL;
        }
    }
    else if (temperature < lowThreshold || temperature > highThreshold)
    {
        _state = SystemState::WARNING;
    }
    // A click from NORMAL/WARNING never acknowledges a newly entered alarm.
    if (acknowledge && wasAlarm && _state == SystemState::ALARM)
    {
        _state = SystemState::ACKNOWLEDGED;
    }
    return _state;
}
SystemState AlarmController::state() const
{
    return _state;
}
const char *AlarmController::name(SystemState state)
{
    switch (state)
    {
    case SystemState::NORMAL:
        return "NORMAL";
    case SystemState::WARNING:
        return "WARNING";
    case SystemState::ALARM:
        return "ALARM";
    default:
        return "ACK";
    }
}
