
#include "debug.h"
#include "timer.h"
#include "libc_test.h"
#include <stdio.h>

#include "arm.h"

void thread1()
{
    while (1)
    {
        printf("111");
    }
}

void thread2()
{
    while (1)
    {
        printf("222");
    }
}

int main()
{
    debug_init();

    libc_test();

    sys_tick_init();

    /* Make sure that the PSP is valid */
    psp_set(msp_get());

    /* Set the thread mode stack to be the PSP */
    control_set(0x00000002);

    /* Enter the OS */
    svc();

    while (1);
}
