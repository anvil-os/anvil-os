
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "debug.h"

static int callback(__attribute__((unused)) void *dest, const char *str, int len)
{
    int n;
    for (n = 0; n < len; ++n)
    {
        debug_putc(str[n]);
    }
    return len;
}

int vprintf(const char *restrict format, va_list arg)
{
    return _Anvil_printf(format, arg, callback, NULL);
}
