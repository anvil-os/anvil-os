
#include <stdio.h>
#include <stdarg.h>

int printf(const char *restrict format, ...)
{
    int ret;
    va_list ap;
    va_start(ap, format);
    ret = vprintf(format, ap);
    va_end(ap);
    return ret;
}
