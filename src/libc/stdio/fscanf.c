
#include <stdio.h>
#include <stdarg.h>

int fscanf(FILE *restrict stream, const char *restrict format, ...)
{
    int ret;
    va_list ap;
    va_start(ap, format);
    ret = vfscanf(stream, format, ap);
    va_end(ap);
    return ret;
}
