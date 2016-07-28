/* Test which brings default HelloWorld project from mbed online compiler
   to be built under GCC.
*/
#include "mbed.h"

Serial pc(SERIAL_TX, SERIAL_RX);
DigitalOut myled(LED1);
I2C i2c(I2C_SDA, I2C_SCL);

int main()
{
    pc.printf("I2C Scanner !\r\n");


    while(1)
        {
            uint8_t error, address;
            int nDevices;

            pc.puts("Scanning...");

            for(int i = 0; i < 128 ; i++) {
                i2c.start();
                if(i2c.write(i << 1)) pc.printf("0x%x ACK \r\n",i); // Send command string
                i2c.stop();
            }
        }
}
