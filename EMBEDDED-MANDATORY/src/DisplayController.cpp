#include "DisplayController.h"
#include <cstdio>
#include <cstring>
DisplayController::DisplayController(PinName sda,PinName scl):_lcd(sda,scl){}
void DisplayController::line(int row,const char* text){
    char buffer[17];const size_t size=std::strlen(text);
    for(int i=0;i<16;i++)buffer[i]=size_t(i)<size?text[i]:' ';
    buffer[16]='\0';_lcd.locate(0,char(row));_lcd.print(buffer);
}
void DisplayController::update(const DisplayData& d,uint64_t now){
    switch(d.state){
    case SystemState::NORMAL:_lcd.setRGB(0,char(255),0);break;
    case SystemState::WARNING:_lcd.setRGB(char(255),char(180),0);break;
    case SystemState::ALARM:_lcd.setRGB(char(255),0,0);break;
    default:_lcd.setRGB(char(150),0,char(255));break;
    }
    char first[40],second[40];
    if(!d.valid)std::snprintf(first,sizeof first,"TEMP ERROR %s",AlarmController::name(d.state));
    else std::snprintf(first,sizeof first,"T:%4.1fC %s",d.temperature,AlarmController::name(d.state));
    const unsigned page=(now/2000)%3;
    if(page==0)std::snprintf(second,sizeof second,"L:%3.0f%% LED:%3.0f%%",d.lightPercent,d.ledPercent);
    else if(page==1)std::snprintf(second,sizeof second,"Lo:%4.1f Hi:%4.1f",d.low,d.high);
    else std::snprintf(second,sizeof second,"%s %s K:%4.1f",d.editingHigh?"HI":"LO",d.captured?"SET":"PICK",d.knob);
    line(0,first);line(1,second);
}
