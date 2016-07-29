#include "mbed.h"
#include "rtos.h"

DigitalOut led1(LED1);
RawSerial pc(SERIAL_TX, SERIAL_RX);

void u1_thread(void const *args) {
    while (true) {
        pc.printf("Tread 1\r\n");
        Thread::wait(1000);
    }
}

void u2_thread(void const *args) {
    while (true) {
        pc.printf("Tread 2\r\n");
        Thread::wait(750);
    }
}
int main() {

    pc.printf("Hello World !\r\n");

    Thread thread1(u1_thread);
    Thread thread2(u2_thread);
    while (true) {
        led1 = !led1;
        Thread::wait(250);
    }
}
