#include "mbed.h"

//------------------------------------
// Hyperterminal configuration
// 9600 bauds, 8-bit data, no parity
//------------------------------------

RawSerial pc(SERIAL_TX, SERIAL_RX);

DigitalOut myled(LED1);

int main() {
  int i = 1;
  //pc.printf("Hello World !\r\n");
  while(1) {
      wait(1);
      pc.putc('A');
      //pc.printf("This program runs since %d seconds.\r\n", i++);
      myled = !myled;
  }
}
