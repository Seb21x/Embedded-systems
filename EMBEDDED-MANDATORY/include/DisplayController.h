#pragma once
#include "AlarmController.h"
#include "CalendarClock.h"
#include "Grove_LCD_RGB_Backlight.h"
#include <cstdint>

struct DisplayData
{
    float temperature, lightPercent, low, high;
    bool valid, editingHigh;
    SystemState state;
};

class DisplayController
{
public:
    DisplayController(PinName sda, PinName scl);
    void update(const DisplayData &data, const CalendarClock &clock, uint64_t nowMs);

private:
    void line(int row, const char *text);
    Grove_LCD_RGB_Backlight _lcd;
    char _previous[2][17] = {};
    int _previousState = -1;
};
