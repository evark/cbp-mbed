/*
 *  Copyright (c) 2012 Neal Horman - http://www.wanlink.com
 *
 *  License: MIT open source (http://opensource.org/licenses/MIT)
 *      Summary;
 *      Use / modify / distribute / publish it how you want and
 *      if you use it, or don't, you can't hold me liable for how
 *      it does or doesn't work.
 *      If it doesn't work how you want, don't use it, or change
 *      it so that it does work.
 *
 *      Modified for use with FRDM-K64F and FRDM-KL25Z by Francesco Adamo
 */

#include "mbed.h"
#include "Adafruit_SSD1306.h"


DigitalOut myled(LED1);
Adafruit_SSD1306_I2C display(I2C_SDA, I2C_SCL, PC_5, 0x3C, 64, 128);

int main() {
    int k = 0;
    display.begin(2);
    display.splash();
    wait(2.0);
    display.clearDisplay();

    display.printf("%ux%u OLED Display\r\n", display.width(), display.height());
    while(1)
    {
        myled = !myled;
        display.printf("%u\r", k++);
        display.display();
        wait(0.1);
    }
}
