#include "mbed.h"
#include "AlarmController.h"
#include "ThresholdModel.h"
#include "ThresholdController.h"
#include "MeasurementFilter.h"
#include "BuzzerController.h"
#include "LightingController.h"
#include "AlarmButton.h"
#include "TempSensor.h"
#include "DisplayController.h"
#include <cassert>
#include <cmath>
#include <iostream>
namespace fake {
std::array<float,64> analog{};std::array<int,64> digital{};
std::array<float,64> pwm{};std::array<int,64> period{};
std::array<std::function<void()>,64> rise{},fall{};
std::vector<std::string> writes;
}
using S=SystemState;
static void near(float a,float b){assert(std::fabs(a-b)<0.001f);}
int main(){
    AlarmController a;
    assert(a.update(24,18,28,true,false)==S::NORMAL);
    assert(a.update(28.1f,18,28,true,false)==S::WARNING);
    assert(a.update(28,18,28,true,false)==S::WARNING);
    assert(a.update(27.5f,18,28,true,false)==S::NORMAL);
    assert(a.update(31,18,28,true,true)==S::ALARM); // stale click cannot acknowledge
    assert(a.update(30.6f,18,28,true,false)==S::ALARM);
    assert(a.update(30.5f,18,28,true,false)==S::WARNING);
    assert(a.update(15,18,28,true,false)==S::ALARM);
    assert(a.update(15.4f,18,28,true,false)==S::ALARM);
    assert(a.update(15.5f,18,28,true,false)==S::WARNING);
    assert(a.update(14,18,28,true,false)==S::ALARM);
    assert(a.update(14,18,28,true,true)==S::ACKNOWLEDGED);
    assert(a.update(18,18,28,true,false)==S::ACKNOWLEDGED);
    assert(a.update(18.5f,18,28,true,false)==S::NORMAL);
    assert(a.update(32,18,28,true,false)==S::ALARM);
    assert(a.update(24,18,28,true,false)==S::NORMAL);
    assert(a.update(NAN,18,28,false,false)==S::ALARM);
    assert(a.update(NAN,18,28,false,true)==S::ACKNOWLEDGED);
    assert(a.update(24,18,28,true,false)==S::NORMAL);
    ThresholdModel th;
    th.update(0,false);near(th.low,18);assert(!th.captured);
    th.update(8.0f/30,false);near(th.low,18);assert(th.captured);
    th.update(1,false);near(th.low,26);
    th.update(1,true);near(th.high,28);assert(!th.captured);
    th.update(0.6f,true);assert(th.captured);
    th.update(0,true);near(th.high,28); // cannot cross low+2
    // Random walks: both bounds and gap always hold.
    unsigned seed=1;
    for(int i=0;i<10000;i++){
        seed=seed*1664525u+1013904223u;
        th.update(float(seed%1001)/1000,(seed&0x10000)!=0);
        assert(th.low>=10 && th.high<=40 && th.high-th.low>=2);
    }
    MeasurementFilter filter;near(filter.update(25),25);near(filter.update(30),26);
    BuzzerController b(D9);assert(fake::period[D9]==500);
    b.update(S::WARNING,0);near(fake::pwm[D9],0.5f);
    b.update(S::WARNING,100);near(fake::pwm[D9],0);
    b.update(S::WARNING,1000);near(fake::pwm[D9],0.5f);
    b.update(S::ALARM,1020);near(fake::pwm[D9],0.5f);
    b.update(S::ALARM,1170);near(fake::pwm[D9],0);
    b.update(S::ACKNOWLEDGED,1180);near(fake::pwm[D9],0);
    LightingController light(D3,0.1f,0.9f);assert(fake::period[D3]==1000);
    light.update(0.74f);near(light.percentage(),80);near(light.duty(),0.2f);
    light.update(0);near(light.duty(),1);light.update(1);near(light.duty(),0);
    TempSensor temp(A0);fake::analog[A0]=0.5f;near(temp.getTemperatureCelsius(),25);
    fake::analog[A0]=0;assert(std::isnan(temp.getTemperatureCelsius()));
    fake::analog[A0]=1;assert(std::isnan(temp.getTemperatureCelsius()));
    fake::analog[A0]=0.6f;assert(temp.getTemperatureCelsius()>25);
    AlarmButton button(PC_13,1);
    fake::digital[PC_13]=1;fake::rise[PC_13]();assert(!button.poll(0,false));
    assert(!button.poll(100,true)); // held normal-state press is not queued
    fake::digital[PC_13]=0;button.poll(110,true);button.poll(160,true);
    fake::digital[PC_13]=1;fake::rise[PC_13]();assert(!button.poll(200,true));
    fake::rise[PC_13]();assert(!button.poll(220,true));
    assert(button.poll(250,true));assert(!button.poll(300,true));
    fake::digital[PC_13]=0;button.poll(310,true);button.poll(360,true);
    fake::digital[PC_13]=1;fake::rise[PC_13]();assert(!button.poll(400,true));
    fake::digital[PC_13]=0;assert(!button.poll(410,true));assert(!button.poll(460,true));
    ThresholdController hw(A2,D2);fake::analog[A2]=8.0f/30;
    fake::digital[D2]=0;hw.update(0);assert(!hw.isEditingHigh());
    fake::digital[D2]=1;hw.update(10);hw.update(59);assert(!hw.isEditingHigh());
    hw.update(60);assert(hw.isEditingHigh());
    DisplayController display(D14,D15);
    DisplayData d={24.5f,80,20,18,28,18,true,false,false,S::WARNING};
    for(unsigned time:{0u,2000u,4000u}){
        fake::writes.clear();display.update(d,time);
        std::string chars;
        for(auto& w:fake::writes)if(w.size()==2 && w[0]==0x40)chars+=w[1];
        assert(chars.size()==32);assert(chars.substr(0,16).find("WARNING")!=std::string::npos);
        for(char c:chars)assert(c>=32 && c<=126);
    }
    std::cout<<"PASS: FSM, hysteresis, ACK/rearm, faults, thresholds (10000 walks), filter, PWM, ADC, debounce, LCD rows\n";
}
