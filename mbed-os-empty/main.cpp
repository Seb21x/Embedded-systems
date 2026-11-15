#include "mbed.h"
#include <iostream>
#include "Flasher.h"
DigitalOut pin(LED2);
AnalogIn testIn(A0);
/*class Flasher {
    public:
    Flasher(PinName pin) : _pin(pin){
        _pin=0;
    }

    void flash(int n){

        for(int i=0;i<n*2;i++){
            _pin= !_pin;
            ThisThread::sleep_for(500ms);
        }
    }
    private:
    DigitalOut _pin;


};*/

Flasher led(LED1);
Flasher led2(LED2);

/*void flash(int n){

    for(int i=0;i<n*2;i++){
        pin= !pin;
        ThisThread::sleep_for(500ms);
    }
}*/
// main() runs in its own thread in the OS
int main()
{
    // led.flash(5);
    // led2.flash(2);
    //testIn.read() - voltage 0 = 0V; 1 = 3.3V 
    float temp = testIn.read();
    cout << "Tempeeeeeratuuuuura  " << temp << endl;
}

