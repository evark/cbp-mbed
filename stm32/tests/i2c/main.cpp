#include "mbed.h"

I2C i2c(I2C_SDA , I2C_SCL );
Serial pc(SERIAL_TX, SERIAL_RX);

int main() {
    pc.printf("\nI2C Scanner");
    //i2c.frequency(100000);
    while(1) {
        int error, address;
        int nDevices;

        pc.printf("Scanning...\n");

         nDevices = 0;

          for(address = 1; address < 127; address++ )
          {
            i2c.start();
            error = i2c.write(address << 1); //We shift it left because mbed takes in 8 bit addreses
            i2c.stop();
            if (error == 1)
            {
              pc.printf("I2C device found at address 0x%X", address); //Returns 7-bit addres
              nDevices++;
            }

          }
          if (nDevices == 0)
            pc.printf("No I2C devices found\n");
          else
            pc.printf("\ndone\n");

          wait(5);           // wait 5 seconds for next scan

            }
        }
