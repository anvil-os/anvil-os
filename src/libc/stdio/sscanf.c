
#include <stdio.h>
#include <stdarg.h>

int sscanf(const char *restrict s, const char *restrict format, ...)
{
    int ret;
    va_list ap;
    va_start(ap, format);
    ret = vsscanf(s, format, ap);
    va_end(ap);
    return ret;
}
