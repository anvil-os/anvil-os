
#include "debug.h"

#include "libc_test.h"

int libc_test()
{
    string_test();
    ctype_test();
    stdlib_test();
    stdio_printf_test();
    stdlib_malloc_test();
    time_test();

    return 0;
}
