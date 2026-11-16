#pragma once
#include "Grove_LCD_RGB_Backlight.h"
#include "AlarmController.h"
#include <cstdint>
struct DisplayData {
    float temperature,lightPercent,ledPercent,low,high,knob;
    bool valid,editingHigh,captured;
    SystemState state;
};
class DisplayController {
public:
    DisplayController(PinName sda,PinName scl);
    void update(const DisplayData& data,uint64_t nowMs);
private:
    void line(int row,const char* text);
    Grove_LCD_RGB_Backlight _lcd;
};
