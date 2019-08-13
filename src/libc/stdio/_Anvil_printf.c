
#include <stdarg.h>

int _Anvil_printf(const char *fmt, va_list ap, int (*nputs)(void *, const char *, int), void *arg)
{
    fmt = fmt;
    ap = ap;
    nputs = nputs;
    arg = arg;
    return 0;
}
