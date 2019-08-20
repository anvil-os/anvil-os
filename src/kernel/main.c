
#include "debug.h"
#include "timer.h"
#include "libc_test.h"
#include <stdio.h>

int main()
{
    debug_init();

    libc_test();

    sys_tick_init();

    while (1);
}
