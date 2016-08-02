#include "mbed.h"
#include "rtos.h"

DigitalOut led1(LED1);
RawSerial pc(SERIAL_TX, SERIAL_RX);
Mutex pc_mutex;

extern volatile uint32_t os_time;  // This is RTOS Ticker

void u1_thread(void const *args) {
    while (true) {
        pc_mutex.lock();
        pc.putc('1');
        pc_mutex.unlock();
        Thread::wait(75);
    }
}

void u2_thread(void const *args) {
    while (true) {
        pc_mutex.lock();
        pc.putc('2');
        pc_mutex.unlock();
        Thread::wait(500);
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
        pc_mutex.lock();
        pc.printf("\n Tick:%d", os_time);
        pc.printf("\n(Thread 1 stack used/total:%d/%d)", thread1.used_stack(),thread1.stack_size());
        pc.printf("\n(Thread 2 stack used/total:%d/%d)\n", thread2.used_stack(),thread2.stack_size());
        pc_mutex.unlock();
        Thread::wait(1000);
    }
}
