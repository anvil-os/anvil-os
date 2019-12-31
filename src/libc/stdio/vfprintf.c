
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "debug.h"

static int callback(void *dest, __attribute__((unused)) const char *str, int len)
{
    FILE *stream = (FILE *)dest;
    size_t nchars = len;

    while (nchars)
    {
        if (_Anvil_fputc_unlocked(*str, stream) == EOF)
        {
            // Todo: this doesn't work. We need to tell _Anvil_printf there
            // was an error so it can return an error
            return EOF;
        }
        ++str;
        --nchars;
    }
    return len;
}

int vfprintf(FILE *restrict stream, const char *restrict format, va_list arg)
{
    int ret;
    _Anvil_flockfile(stream);
    ret = _Anvil_printf(format, arg, callback, stream);
    _Anvil_funlockfile(stream);
    return ret;
}
