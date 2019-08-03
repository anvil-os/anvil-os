
#include "debug.h"
#include "libc_test.h"

int main()
{
    debug_init();

    libc_test();

    while (1);
}
