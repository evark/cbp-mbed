#include "mbed.h"
#include <Timer64.h>

Timer64     tm64;

int main()
{
    uint64_t timeInUsec;

    tm64.init();
    tm64.start();

    int Value = -1;


    printf("\r\n\r\nMBED Test Suite\r\n");
    printf("Standard I/O Unit Tests\r\n");

    printf("ClockFreq: %lu\r\n",HAL_RCC_GetSysClockFreq() );
    printf("PCLK1Freq: %lu\r\n",HAL_RCC_GetPCLK1Freq());
    printf("PCLK2Freq: %lu\r\n",HAL_RCC_GetPCLK2Freq());
    printf("HCLKFreq:  %lu\r\n",HAL_RCC_GetHCLKFreq());
    for(;;)
    {
        tm64.read_us(&timeInUsec);
        printf("%d %llu\r\n",Value++, timeInUsec);
        Thread::wait(100);
    }
}
