
#include <stdio.h>
#include <stdarg.h>

int fprintf(FILE *restrict stream, const char *restrict format, ...)
{
    int ret;
    va_list ap;
    va_start(ap, format);
    ret = vfprintf(stream, format, ap);
    va_end(ap);
    return ret;
}
