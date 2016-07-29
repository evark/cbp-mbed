#include "mbed.h"
#include "rtos.h"

DigitalOut led1(LED1);
RawSerial pc(SERIAL_TX, SERIAL_RX);
int i=0;

void u1_thread(void const *args) {
    while (true) {
        pc.printf("Tread 1\r\n");
        Thread::wait(500);
    }
}

void u2_thread(void const *args) {
    while (true) {
        pc.printf("Tread 2\r\n");
        Thread::wait(750);
    }
}

void t1_timer(void const *args) {
        led1 = !led1;
}

int main() {

    pc.printf("Hello World !\r\n");

    Thread thread1(u1_thread);
    Thread thread2(u2_thread);

    RtosTimer timer1(t1_timer, osTimerPeriodic);

    timer1.start(100);

    while (true) {
        pc.printf("Thread 1 stack used/total:%d/%d\r\n", thread1.max_stack(),thread1.stack_size());
        pc.printf("Thread 2 stack used/total:%d/%d\r\n", thread2.max_stack(),thread2.stack_size());
        Thread::wait(1000);
    }
}
