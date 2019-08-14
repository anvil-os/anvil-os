
#include <stdarg.h>

int _Anvil_printf(const char *fmt, va_list ap, int (*nputs)(void *, const char *, int), void *arg)
{
    ap = ap;

    int chars_printed = 0;

    while (*fmt)
    {
        /* Collect up and print any non-% chars */
        const char *str = fmt;
        while (*fmt && (*fmt != '%'))
        {
            ++fmt;
        }
        if (fmt > str)
        {
            chars_printed += nputs(arg, str, fmt - str);
        }

        /* If we get here, *fmt is a '%' or a '\0' */
        if (*fmt == '\0')
        {
            return chars_printed;
        }
    }

    return 0;
}
