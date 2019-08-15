
#include <stdarg.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#define FLAG_LEFT_JUSTIFY       (1)
#define FLAG_SHOW_SIGN          (2)
#define FLAG_SPACE              (4)
#define FLAG_ALTERNATIVE        (8)
#define FLAG_ZEROPAD            (16)
#define FLAG_CHAR               (32)
#define FLAG_SHORT              (64)
#define FLAG_LONG               (128)
#define FLAG_LONGLONG           (256)
#define FLAG_UNSIGNED           (512)
#define FLAG_OCTAL              (1024)
#define FLAG_HEX                (2048)
#define FLAG_UPPER              (4096)

static int print_str(int (*nputs)(void *, const char *, int ), void *arg, va_list *ap, int field_width, int precision, int flags);
static int print_num(int (*nputs)(void *, const char *, int ), void *arg, va_list *ap, int field_width, int precision, int flags);
static const char lower_digit[] = "0123456789abcdef";
static const char upper_digit[] = "0123456789ABCDEF";
static const char space[] = " ";

int _Anvil_printf(const char *fmt, va_list ap, int (*nputs)(void *, const char *, int), void *arg)
{
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
                ++fmt;
                if (*fmt == 'h')
                {
                    flags |= FLAG_CHAR;
                    ++fmt;
                }
                else
                {
                    flags |= FLAG_SHORT;
                }
                break;
            case 'l':
                ++fmt;
                if (*fmt == 'l')
                {
                    flags |= FLAG_LONGLONG;
                    ++fmt;
                }
                else
                {
                    flags |= FLAG_LONG;
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
                 chars_printed += print_num(nputs, arg, &ap, field_width, precision, flags);
                 break;
             case 'o':
                 chars_printed += print_num(nputs, arg, &ap, field_width, precision, flags | FLAG_UNSIGNED | FLAG_OCTAL);
                 break;
             case 'u':
                 chars_printed += print_num(nputs, arg, &ap, field_width, precision, flags | FLAG_UNSIGNED);
                 break;
             case 'x':
                 chars_printed += print_num(nputs, arg, &ap, field_width, precision, flags | FLAG_UNSIGNED | FLAG_HEX);
                 break;
             case 'X':
                 chars_printed += print_num(nputs, arg, &ap, field_width, precision, flags | FLAG_UNSIGNED | FLAG_HEX | FLAG_UPPER);
                 break;
             case 'f':
             case 'F':
             case 'e':
             case 'E':
             case 'g':
             case 'G':
             case 'a':
             case 'A':
             case 'c':
                 break;
             case 's':
                 chars_printed += print_str(nputs, arg, &ap, field_width, precision, flags);
                 break;
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

    return chars_printed;
}

static int print_str(int (*nputs)(void *, const char *, int ), void *arg, va_list *ap, int field_width, int precision, int flags)
{
    /* For a string:
     *  field_width is minimum field width
     *  precision is the maximum chars that can be printed
     *  FLAG_LEFT_JUSTIFY is the only flag that matters
     *  Todo: FLAG_LONG is not yet supported
     */

    int chars_printed = 0;
    int string_len;
    int pad_len;

    const char *p_str = va_arg(*ap, const char *);

    /* Assume we will print the entire string */
    string_len = strlen(p_str);

    /* If a precision is given we limit to that */
    if (string_len > precision)
    {
        string_len = precision;
    }

    /* We need to pad out to the field width */
    if (field_width > string_len)
    {
        pad_len = field_width - string_len;
    }
    else
    {
        pad_len = 0;
    }

    if (flags & FLAG_LEFT_JUSTIFY)
    {
        /* Print the string with trailing spaces */
        chars_printed += nputs(arg, p_str, string_len);
        while (pad_len--)
        {
            chars_printed += nputs(arg, space, 1);
        }
    }
    else
    {
        /* If right justified print leading spaces */
        while (pad_len--)
        {
            chars_printed += nputs(arg, space, 1);
        }
        /* Print the string */
        chars_printed += nputs(arg, p_str, string_len);
    }

    return chars_printed;
}

static int print_num(int (*nputs)(void *, const char *, int ), void *arg, va_list *ap, int field_width, int precision, int flags)
{
    int chars_printed = 0;
    int num_len;
    char buf[20];
    char *p_num;
    unsigned long long ullval;
    int neg = 0;
    unsigned radix;
    const char *digit;
    char sign_char;

    /****************************************
     * Read value depending on the length
     * modifier and sign
     ***************************************/
    if (flags & FLAG_UNSIGNED)
    {
        if (flags & FLAG_CHAR)
        {
            ullval = (unsigned char)va_arg(*ap, unsigned int);
        }
        else if (flags & FLAG_SHORT)
        {
            ullval = (unsigned short)va_arg(*ap, unsigned int);
        }
        else if (flags & FLAG_LONG)
        {
            ullval = va_arg(*ap, unsigned long);
        }
        else if (flags & FLAG_LONGLONG)
        {
            ullval = va_arg(*ap, unsigned long long);
        }
        else
        {
            ullval = va_arg(*ap, unsigned int);
        }
    }
    else
    {
        long long llval;
        if (flags & FLAG_CHAR)
        {
            llval = (char)va_arg(*ap, int);
        }
        else if (flags & FLAG_SHORT)
        {
            llval = (short)va_arg(*ap, int);
        }
        else if (flags & FLAG_LONG)
        {
            llval = va_arg(*ap, long);
        }
        else if (flags & FLAG_LONGLONG)
        {
            llval = va_arg(*ap, long long);
        }
        else
        {
            llval = va_arg(*ap, int);
        }
        if (llval < 0)
        {
            ullval = -llval;
            neg = 1;
        }
        else
        {
            ullval = llval;
        }
    }

    /****************************************
     * Set the radix
     ***************************************/
    if (flags & FLAG_HEX)
    {
        radix = 16;
    }
    else if (flags & FLAG_OCTAL)
    {
        radix = 8;
    }
    else
    {
        radix = 10;
    }

    /****************************************
     * Convert the number to a string
     ***************************************/
    digit = flags & FLAG_UPPER ? upper_digit : lower_digit;

    p_num = &buf[20];
    num_len = 0;
    while (ullval || num_len == 0)
    {
        *--p_num = digit[ullval % radix];
        ullval /= radix;
        ++num_len;
    }

    /****************************************
     * The sign char
     ***************************************/
    sign_char = 0;

    if (!(flags & FLAG_UNSIGNED))
    {
        if (neg)
        {
            sign_char = '-';
        }
        else
        {
            if (flags & FLAG_SHOW_SIGN)
            {
                sign_char = '+';
            }
            else if (flags & FLAG_SPACE)
            {
                sign_char = ' ';
            }
        }
    }

    if (sign_char)
    {
        chars_printed += nputs(arg, &sign_char, 1);
    }

    chars_printed += nputs(arg, p_num, num_len);

    return chars_printed;
}
