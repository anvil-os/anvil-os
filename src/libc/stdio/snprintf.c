
#include <stdio.h>
#include <stdarg.h>

int snprintf(char *restrict s, size_t n, const char *restrict format, ...)
{
    int ret;
    va_list ap;
    va_start(ap, format);
    ret = vsnprintf(s, n, format, ap);
    va_end(ap);
    return ret;
}
