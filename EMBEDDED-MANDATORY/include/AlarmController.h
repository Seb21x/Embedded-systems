#pragma once
#include <cstdint>
enum class SystemState
{
    NORMAL,
    WARNING,
    ALARM,
    ACKNOWLEDGED
};
class AlarmController
{
public:
    SystemState update(float temperature, float low, float high, bool valid, bool acknowledge);
    SystemState state() const;
    static const char *name(SystemState state);

private:
    SystemState _state = SystemState::NORMAL;
};
