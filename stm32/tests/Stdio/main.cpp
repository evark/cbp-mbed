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

    printf("\r\n\r\nGCC4MBED Test Suite\r\n");
    printf("Standard I/O Unit Tests\r\n");

    printf("ClockFreq: %d\n",HAL_RCC_GetSysClockFreq() );

    for(;;)
    {
        printf("%d ",Value++);
    }
}
