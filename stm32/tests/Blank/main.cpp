/* Test which brings default HelloWorld project from mbed online compiler
   to be built under GCC.
*/
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
