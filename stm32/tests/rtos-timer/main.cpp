#include "mbed.h"
#include "rtos.h"

DigitalOut led1(LED1);
RawSerial pc(SERIAL_TX, SERIAL_RX);

extern volatile uint32_t os_time;  // This is RTOS Ticker


void t1_timer(void const *args) {
        led1 = !led1;
        pc.printf("\n---Tick1:%d", os_time);
}
void t2_timer(void const *args) {
        led1 = !led1;
        pc.printf("\n------Tick2:%d", os_time);
}

int main() {

    pc.printf("Hello World !\r\n");

    RtosTimer timer1(t1_timer, osTimerPeriodic);
    RtosTimer timer2(t2_timer, osTimerPeriodic);

    timer1.start(100);
    timer2.start(500);

    Thread::wait(osWaitForever);
}
