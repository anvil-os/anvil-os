
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "debug.h"

static int callback(void *p, const char *str, int len)
{
    memcpy(*(char **)p, str, len);
    *(char **)p += len;
    /* We need to return the number of chars copied */
    return len;
}

int vsprintf(char *restrict s, const char *restrict format, va_list arg)
{
    char *p = s;
    int ret;
    ret = _Anvil_printf(format, arg, callback, &p);
    if (ret >= 0)
    {
        s[ret] = 0;
    }
    return ret;
}
