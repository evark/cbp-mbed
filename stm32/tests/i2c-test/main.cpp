#include "mbed.h"

I2C i2c(I2C_SDA , I2C_SCL );
Serial pc(SERIAL_TX, SERIAL_RX);
DigitalOut myled(LED1);
const uint8_t Address=0x7C;

uint8_t command(uint8_t c)
	{
		uint8_t buff[2];
		buff[0] = 0; // Command Mode
		buff[1] = c;
		i2c.write(Address, (char*)buff, sizeof(buff));

		return c;
	}

void data(uint8_t c)
	{
		uint8_t buff[2];
		buff[0] = 0x40; // Data Mode
		buff[1] = c;
		i2c.write(Address, (char*)buff, sizeof(buff));
	};

int main()
{
    pc.printf("\nI2C Test");
    i2c.start();
    command(0xAE);
    command(0xD5);
    command(0x80);
    command(0xA8);
    command(0x80);
    command(64);
    while(1){};
}
