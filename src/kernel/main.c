
#include "debug.h"
#include "timer.h"
#include "libc_test.h"
#include <stdio.h>
#include <stdlib.h>
#include <threads.h>

#include "arm.h"

int thread2(void *arg)
{
    arg = arg;
    while (1)
    {
        printf("222");
    }
}

int thread3(void *arg)
{
    arg = arg;
    while (1)
    {
        printf("333");
    }
}

int main()
{
    thrd_t thr;

    debug_init();

    libc_test();

    while (1);

    sys_tick_init();

    thrd_create(&thr, thread2, (void *)0x12345678);
    thrd_create(&thr, thread3, (void *)0x12345678);

    while (1)
    {
        printf("111");
    }
}
