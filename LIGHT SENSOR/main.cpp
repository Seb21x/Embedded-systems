#include "mbed.h"

AnalogIn photoresistor(A0);
DigitalOut led(A1);
AnalogIn threshold(A3);
InterruptIn btn(BUTTON1);

float thresh_low = 0.4f;
float thresh_high = 0.6f;

volatile int edit_mode = 0;

void on_button_press() {
    edit_mode = !edit_mode;
    printf("Tryb edycji: %s\n", edit_mode ? "Prog GORNY" : "Prog DOLNY");
}

int main()
{
    btn.fall(&on_button_press);
    while (true) {
        float pot_val = threshold.read();

        if (edit_mode == 0) {
            thresh_low = pot_val;
            if (thresh_low >= thresh_high) thresh_low = thresh_high - 0.05f;
        } else {
            thresh_high = pot_val;
            if (thresh_high <= thresh_low) thresh_high = thresh_low + 0.05f;
        }

        float light = photoresistor.read();

        if (light < thresh_low) {
            led = 1;
        } 
        else if (light > thresh_high) {
            led = 0;
        }

        printf("Light: %.2f | Low: %.2f | High: %.2f | LED: %d\r\n", 
               light, thresh_low, thresh_high, led.read());
    }
}