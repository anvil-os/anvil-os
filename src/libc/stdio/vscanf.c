
#include <stdio.h>

int vscanf(const char *restrict format, _Anvil_va_list arg)
{
    return vfscanf(stdin, format, arg);
}
