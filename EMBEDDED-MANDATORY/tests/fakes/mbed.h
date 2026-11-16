#pragma once
// Host-only fake hardware. Excluded from Mbed firmware by .mbedignore.
#include <chrono>
#include <functional>
#include <cstdio>
#include <array>
#include <string>
#include <vector>
using PinName=int;
enum {NC=-1,A0,A1,A2,D2,D3,D7,D9,D14,D15,PC_13,LED1,LED2,LED3};
enum PinMode {PullDown,PullUp};
namespace fake {
extern std::array<float,64> analog;
extern std::array<int,64> digital;
extern std::array<float,64> pwm;
extern std::array<int,64> period;
extern std::array<std::function<void()>,64> rise,fall;
extern std::vector<std::string> writes;
}
class AnalogIn {int p;public:AnalogIn(int pin):p(pin){} float read(){return fake::analog[p];}};
class DigitalIn {int p;public:DigitalIn(int pin,PinMode=PullDown):p(pin){}int read(){return fake::digital[p];}};
class DigitalOut {int p;public:DigitalOut(int pin,int initial=0):p(pin){if(p>=0)fake::digital[p]=initial;}void operator=(int v){if(p>=0)fake::digital[p]=v;}};
class PwmOut {int p;public:PwmOut(int pin):p(pin){}void period_us(int x){fake::period[p]=x;}void write(float x){fake::pwm[p]=x;}};
class InterruptIn {int p;public:InterruptIn(int pin):p(pin){}int read(){return fake::digital[p];}void rise(std::function<void()> f){fake::rise[p]=f;}void fall(std::function<void()> f){fake::fall[p]=f;}};
template<class T>std::function<void()> callback(T* p,void(T::*f)()){return [p,f](){(p->*f)();};}
inline void core_util_critical_section_enter(){} inline void core_util_critical_section_exit(){}
inline void wait_us(int){}
namespace ThisThread {template<class T>void sleep_for(T){}}
class Timer {public:void start(){}std::chrono::microseconds elapsed_time(){return std::chrono::microseconds(0);}};
class I2C {public:I2C(int,int){}void frequency(int){}int write(int,const char* data,int n){fake::writes.emplace_back(data,n);return 0;}};
