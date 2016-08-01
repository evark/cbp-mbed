#include "mbed.h"
#include "AM2320.h"

AM2320 am2320(I2C_SDA, I2C_SCL );
Serial pc(SERIAL_TX, SERIAL_RX);
DigitalOut myled(LED1);

float Temperature = 0.0;
float Humidity = 0.0;

int main()
{
    pc.printf("\nI2C AM2320");
    //i2c.frequency(50000);
    while(1) {
        Temperature = am2320.temperature();
        Humidity = am2320.humidity();
        myled = !myled;
        pc.printf("T:%f H:%f\n",Temperature,Humidity);
    }
    //wait(1);
}
