#include "DisplayController.h"
#include <cstdio>
#include <cstring>

DisplayController::DisplayController(PinName sda, PinName scl) : _lcd(sda, scl)
{
}

void DisplayController::line(int row, const char *text)
{
    char buffer[17];
    const size_t length = std::strlen(text);
    for (size_t index = 0; index < 16; ++index)
    {
        buffer[index] = index < length ? text[index] : ' ';
    }
    buffer[16] = '\0';
    if (std::strcmp(buffer, _previous[row]) != 0)
    {
        _lcd.locate(0, static_cast<char>(row));
        _lcd.print(buffer);
        std::memcpy(_previous[row], buffer, sizeof(buffer));
    }
}

void DisplayController::update(const DisplayData &data, const CalendarClock &clock, uint64_t nowMs)
{
    if (_previousState != static_cast<int>(data.state))
    {
        switch (data.state)
        {
        case SystemState::NORMAL:
            _lcd.setRGB(0, char(255), 0);
            break;
        case SystemState::WARNING:
            _lcd.setRGB(char(255), char(180), 0);
            break;
        case SystemState::ALARM:
            _lcd.setRGB(char(255), 0, 0);
            break;
        case SystemState::ACKNOWLEDGED:
            _lcd.setRGB(char(150), 0, char(255));
            break;
        }
        _previousState = static_cast<int>(data.state);
    }
    char first[40], second[40];
    if (data.valid)
    {
        std::snprintf(first, sizeof(first), "T:%4.1fC %s", data.temperature,
                      AlarmController::name(data.state));
    }
    else
    {
        std::snprintf(first, sizeof(first), "TEMP ERROR %s", AlarmController::name(data.state));
    }
    switch ((nowMs / 2000) % 4)
    {
    case 0:
        std::snprintf(second, sizeof(second), "L:%3.0f%% LED:%3.0f%%", data.lightPercent,
                      100.0f - data.lightPercent);
        break;
    case 1:
        std::snprintf(second, sizeof(second), "Lo:%4.1f Hi:%4.1f", data.low, data.high);
        break;
    case 2:
        std::snprintf(second, sizeof(second), "EDIT:%s D2=HIGH", data.editingHigh ? "HIGH" : "LOW");
        break;
    default:
        clock.format(second, sizeof(second), nowMs);
        break;
    }
    line(0, first);
    line(1, second);
}
