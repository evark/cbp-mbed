/* Copyright 2011 Adam Green (http://mbed.org/users/AdamGreen/)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
/* Basic unit test for C Standard I/O routines. */
#include "mbed.h"


int main()
{
    int Value = -1;

    printf("\r\n\r\nMBED Test Suite\r\n");
    printf("Standard I/O Unit Tests\r\n");

    printf("ClockFreq: %lu\r\n",HAL_RCC_GetSysClockFreq() );
    printf("PCLK1Freq: %lu\r\n",HAL_RCC_GetPCLK1Freq());
    printf("PCLK2Freq: %lu\r\n",HAL_RCC_GetPCLK2Freq());
    printf("HCLKFreq:  %lu\r\n",HAL_RCC_GetHCLKFreq());
    for(;;)
    {
        printf("%d ",Value++);
    }
}
