
#include <stdio.h>
#include <stdarg.h>

int sprintf(char *restrict s, const char *restrict format, ...)
{
    int ret;
    va_list ap;
    va_start(ap, format);
    ret = vsprintf(s, format, ap);
    va_end(ap);
    return ret;
}
