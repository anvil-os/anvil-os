
#include <stdarg.h>
#include <stdlib.h>
#include <limits.h>

#define FLAG_LEFT_JUSTIFY       (1)
#define FLAG_SHOW_SIGN          (2)
#define FLAG_SPACE              (4)
#define FLAG_ALTERNATIVE        (8)
#define FLAG_ZEROPAD            (16)
#define FLAG_CHAR               (32)
#define FLAG_SHORT              (64)
#define FLAG_LONG               (128)
#define FLAG_LONGLONG           (256)

int _Anvil_printf(const char *fmt, va_list ap, int (*nputs)(void *, const char *, int), void *arg)
{
    ap = ap;

    int chars_printed = 0;

    while (*fmt)
    {
        const char *str;
        int flags;
        int field_width;
        int precision;

        /****************************************
         * Collect up and print any non-% chars
         ***************************************/
        str = fmt;
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

        /****************************************
         * *fmt is a '%', move over it. After the
         * %, the following appear in sequence:
         ***************************************/
        ++fmt;

        /****************************************
         *  Zero or more flags (in any order)
         ***************************************/
        flags = 0;
        while (1)
        {
            switch (*fmt)
            {
                case '-':
                    flags |= FLAG_LEFT_JUSTIFY;
                    ++fmt;
                    continue;
                case '+':
                    flags |= FLAG_SHOW_SIGN;
                    ++fmt;
                    continue;
                case ' ':
                    flags |= FLAG_SPACE;
                    ++fmt;
                    continue;
                case '#':
                    flags |= FLAG_ALTERNATIVE;
                    ++fmt;
                    continue;
                case '0':
                    flags |= FLAG_ZEROPAD;
                    ++fmt;
                    continue;
                default:
                    break;
            }
            break;
        }

        /****************************************
         *  An optional minimum field width
         ***************************************/
        field_width = 0;
        if (*fmt == '*')
        {
            field_width = va_arg(ap, int);
            ++fmt;
        }
        else
        {
            field_width = strtol(fmt, (char **)&fmt, 10);
        }
        if (field_width < 0)
        {
            /* A negative field width argument is taken as
             * a '-' flag followed by a positive field width.
             */
            field_width = -field_width;
            flags |= FLAG_LEFT_JUSTIFY;
        }

        /****************************************
         *  An optional precision
         ***************************************/
        precision = INT_MAX;
        if (*fmt == '.')
        {
            /* if only the period is specified, the precision is taken as zero */
            precision = 0;
            ++fmt;
            if (*fmt == '*')
            {
                precision = va_arg(ap, int);
                ++fmt;
            }
            else
            {
                precision = strtol(fmt, (char **)&fmt, 10);
            }
            if (precision < 0)
            {
                /* A negative precision argument is taken as if the precision were omitted */
                precision = INT_MAX;
            }
        }

        /****************************************
         *  An optional length modifier
         ***************************************/
        switch (*fmt)
        {
            case 'h':
                flags |= FLAG_SHORT;
                ++fmt;
                if (*fmt == 'h')
                {
                    flags |= FLAG_CHAR;
                    ++fmt;
                }
                break;
            case 'l':
                flags |= FLAG_LONG;
                ++fmt;
                if (*fmt == 'l')
                {
                    flags |= FLAG_LONGLONG;
                    ++fmt;
                }
                break;
            case 'j':
            case 'z':
            case 't':
            case 'L':
                ++fmt;
                break;
         }

        /****************************************
         *  A conversion specifier character
         ***************************************/
        switch (*fmt)
        {
             case 'd':
             case 'i':
             case 'o':
             case 'u':
             case 'x':
             case 'X':
             case 'f':
             case 'F':
             case 'e':
             case 'E':
             case 'g':
             case 'G':
             case 'a':
             case 'A':
             case 'c':
             case 's':
             case 'p':
             case 'n':
                 break;
             case 0:
                 return chars_printed;
             default:
                 break;
         }
         ++fmt;
    }

    return 0;
}
