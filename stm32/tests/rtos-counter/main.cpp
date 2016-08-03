#include "mbed.h"
#include "rtos.h"

uint32_t counter;
bool cReset=0;

RawSerial pc(SERIAL_TX, SERIAL_RX);
Mutex pc_mutex;
AnalogIn   ain(PA_0);
extern volatile uint32_t os_time;  // This is RTOS Ticker

void resetcnt(void const *args)
{
    cReset = 1;
}

void u1_thread(void const *args) {
    while (true) {
        //ain.read_u16();
        pc_mutex.lock();
        pc.printf("[ADC:%u]\n",ain.read_u16());
        pc_mutex.unlock();
        Thread::wait(25);
    }
}

void u2_thread(void const *args) {
    while (true) {
        pc_mutex.lock();
        pc.putc('*');
        pc_mutex.unlock();
        Thread::wait(50);
    }
}

int main()
{
    pc.printf("Hello World !\r\n");

    RtosTimer timerCnt(resetcnt, osTimerPeriodic);
    Thread thread1(u1_thread);
    Thread thread2(u2_thread);

    timerCnt.start(1000);
    while(1) {
            if (cReset) {
                cReset = 0;
                pc_mutex.lock();
                pc.printf("\n[count:%u-%u]", counter,os_time);
                pc_mutex.unlock();
                counter=0;
            }
            counter++;
    }

}
