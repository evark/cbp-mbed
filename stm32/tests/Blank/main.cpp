
#include "mbed.h"
volatile int i=0;
int main()
{
    while(1)
    {
        i++;
        HAL_Delay(50);
    }
}
