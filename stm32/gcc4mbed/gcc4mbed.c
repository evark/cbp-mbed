#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <cmsis.h>

extern unsigned int __bss_start__;
extern unsigned int __bss_end__;
int  main(void);
void __libc_init_array(void);
void software_init_hook(void) __attribute((weak));
void exit(int ErrorCode);
void _start(void)
{
    int bssSize = (int)&__bss_end__ - (int)&__bss_start__;
    int mainReturnValue;

    memset(&__bss_start__, 0, bssSize);

    software_init_hook();
    __libc_init_array();
    mainReturnValue = main();
    exit(mainReturnValue);
}

void __cxa_pure_virtual(void)
{
    exit(1);
}
