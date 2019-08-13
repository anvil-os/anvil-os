
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "debug.h"

static int callback(__attribute__((unused)) void *dest, __attribute__((unused)) const char *str, int len)
{
    return len;
}

int vfprintf(FILE *restrict stream, const char *restrict format, va_list arg)
{
    return _Anvil_printf(format, arg, callback, stream);
}
