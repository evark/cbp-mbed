#include "mbed.h"

DigitalOut myled(LED1);

int main() {

    printf("RTC example\n");
    set_time(1387188323); // Set RTC time to 16 December 2013 10:05:23 UTC
    printf("Date and time are set.\n");

    while(1) {

        time_t seconds = time(NULL);

        //printf("Time as seconds since January 1, 1970 = %d\n", seconds);

        printf("Time as a basic string = %s", ctime(&seconds));

        //char buffer[32];
        //strftime(buffer, 32, "%I:%M:%S %p\n", localtime(&seconds));
        //printf("Time as a custom formatted string = %s", buffer);

        myled = !myled;
        wait(1);
    }
}
