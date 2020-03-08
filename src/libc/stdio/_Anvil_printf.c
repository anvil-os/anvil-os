
#include <stdarg.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>

#define FLAG_SIZE_MASK          (0x000f)
#define FLAG_CHAR_T             (0x0001)
#define FLAG_SHORT_T            (0x0002)
#define FLAG_LONG_T             (0x0003)
#define FLAG_LONGLONG_T         (0x0004)
#define FLAG_INTMAX_T           (0x0005)
#define FLAG_SIZE_T             (0x0006)
#define FLAG_PTRDIFF_T          (0x0007)
#define FLAG_LONG_DBL_T         (0x0008)

#define FLAG_LEFT_JUSTIFY       (0x0010)
#define FLAG_SHOW_SIGN          (0x0020)
#define FLAG_SPACE              (0x0040)
#define FLAG_ALTERNATIVE        (0x0080)
#define FLAG_ZEROPAD            (0x0100)
#define FLAG_UNSIGNED           (0x0200)
#define FLAG_OCTAL              (0x0400)
#define FLAG_HEX                (0x0800)
#define FLAG_UPPER              (0x1000)
#define FLAG_FIXED              (0x2000)
#define FLAG_EXP                (0x4000)

struct printf_ctx
{
    va_list ap;
    int (*nputs)(void *, const char *, int );
    void *arg;
    int field_width;
    int precision;
    int flags;
    int chars_printed;
};

static int print_str(struct printf_ctx *ctx);
static int print_num(struct printf_ctx *ctx);
static int print_double(struct printf_ctx *ctx);
static const char lower_digit[] = "0123456789abcdef";
static const char upper_digit[] = "0123456789ABCDEF";
static const char space[] = " ";
static const char zero[] = "0";

int _Anvil_printf(const char *fmt, va_list ap, int (*nputs)(void *, const char *, int), void *arg)
{
    struct printf_ctx ctx;
    ctx.nputs = nputs;
    ctx.arg = arg;
    ctx.chars_printed = 0;
    va_copy(ctx.ap, ap);

    while (*fmt)
    {
        const char *str;

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
            ctx.chars_printed += nputs(arg, str, fmt - str);
        }

        /* If we get here, *fmt is a '%' or a '\0' */
        if (*fmt == '\0')
        {
            // This will break us out of the main loop and return
            break;
        }

        /****************************************
         * *fmt is a '%', move over it. After the
         * %, the following appear in sequence:
         ***************************************/
        ++fmt;

        /****************************************
         *  Zero or more flags (in any order)
         ***************************************/
        ctx.flags = 0;
        while (1)
        {
            switch (*fmt)
            {
                case '-':
                    ctx.flags |= FLAG_LEFT_JUSTIFY;
                    ++fmt;
                    continue;
                case '+':
                    ctx.flags |= FLAG_SHOW_SIGN;
                    ++fmt;
                    continue;
                case ' ':
                    ctx.flags |= FLAG_SPACE;
                    ++fmt;
                    continue;
                case '#':
                    ctx.flags |= FLAG_ALTERNATIVE;
                    ++fmt;
                    continue;
                case '0':
                    ctx.flags |= FLAG_ZEROPAD;
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
        ctx.field_width = 0;
        if (*fmt == '*')
        {
            ctx.field_width = va_arg(ap, int);
            ++fmt;
        }
        else
        {
            ctx.field_width = strtol(fmt, (char **)&fmt, 10);
        }
        if (ctx.field_width < 0)
        {
            /* A negative field width argument is taken as
             * a '-' flag followed by a positive field width.
             */
            ctx.field_width = -ctx.field_width;
            ctx.flags |= FLAG_LEFT_JUSTIFY;
        }

        /****************************************
         *  An optional precision
         ***************************************/
        ctx.precision = INT_MAX;
        if (*fmt == '.')
        {
            /* if only the period is specified, the precision is taken as zero */
            ctx.precision = 0;
            ++fmt;
            if (*fmt == '*')
            {
                ctx.precision = va_arg(ap, int);
                ++fmt;
            }
            else
            {
                ctx.precision = strtol(fmt, (char **)&fmt, 10);
            }
            if (ctx.precision < 0)
            {
                /* A negative precision argument is taken as if the precision were omitted */
                ctx.precision = INT_MAX;
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
                    ctx.flags |= FLAG_CHAR_T;
                    ++fmt;
                }
                else
                {
                    ctx.flags |= FLAG_SHORT_T;
                }
                break;
            case 'l':
                ++fmt;
                if (*fmt == 'l')
                {
                    ctx.flags |= FLAG_LONGLONG_T;
                    ++fmt;
                }
                else
                {
                    ctx.flags |= FLAG_LONG_T;
                }
                break;
            case 'j':
                ++fmt;
                ctx.flags |= FLAG_INTMAX_T;
                break;
            case 'z':
                ++fmt;
                ctx.flags |= FLAG_SIZE_T;
                break;
            case 't':
                ++fmt;
                ctx.flags |= FLAG_PTRDIFF_T;
                break;
            case 'L':
                ++fmt;
                ctx.flags |= FLAG_LONG_DBL_T;
                break;
        }

        /****************************************
         *  A conversion specifier character
         ***************************************/
        switch (*fmt)
        {
             case 'd':
             case 'i':
                 print_num(&ctx);
                 break;
             case 'o':
                 ctx.flags |= (FLAG_UNSIGNED | FLAG_OCTAL);
                 print_num(&ctx);
                 break;
             case 'u':
                 ctx.flags |= FLAG_UNSIGNED;
                 print_num(&ctx);
                 break;
             case 'x':
                 ctx.flags |= (FLAG_UNSIGNED | FLAG_HEX);
                 print_num(&ctx);
                 break;
             case 'X':
                 ctx.flags |= (FLAG_UNSIGNED | FLAG_HEX | FLAG_UPPER);
                 print_num(&ctx);
                 break;
             case 'f':
                 ctx.flags |= FLAG_FIXED;
                 print_double(&ctx);
                 break;
             case 'F':
                 ctx.flags |= (FLAG_FIXED | FLAG_UPPER);
                 print_double(&ctx);
                 break;
             case 'e':
                 ctx.flags |= FLAG_EXP;
                 print_double(&ctx);
                 break;
             case 'E':
                 ctx.flags |= (FLAG_EXP | FLAG_UPPER);
                 print_double(&ctx);
                 break;
             case 'g':
                 print_double(&ctx);
                 break;
             case 'G':
                 ctx.flags |= FLAG_UPPER;
                 print_double(&ctx);
                 break;
             case 'a':
                 ctx.flags |= FLAG_HEX;
                 print_double(&ctx);
                 break;
             case 'A':
                 ctx.flags |= (FLAG_HEX | FLAG_UPPER);
                 print_double(&ctx);
                 break;
             case 'c':
                 break;
             case 's':
                 print_str(&ctx);
                 break;
             case 'p':
             case 'n':
                 break;
             default:
                 break;
        }
        if (*fmt == '\0')
        {
            // This will break us out of the main loop and return
            break;
        }
        ++fmt;
    }

    va_end(ctx.ap);

    return ctx.chars_printed;
}

static int print_str(struct printf_ctx *ctx)
{
    /* For a string:
     *  field_width is minimum field width
     *  precision is the maximum chars that can be printed
     *  FLAG_LEFT_JUSTIFY is the only flag that matters
     *  Todo: FLAG_LONG is not yet supported
     */

    int string_len;
    int pad_len;

    const char *p_str = va_arg(ctx->ap, const char *);

    /* Assume we will print the entire string */
    string_len = strlen(p_str);

    /* If a precision is given we limit to that */
    if (string_len > ctx->precision)
    {
        string_len = ctx->precision;
    }

    /* We need to pad out to the field width */
    if (ctx->field_width > string_len)
    {
        pad_len = ctx->field_width - string_len;
    }
    else
    {
        pad_len = 0;
    }

    if (ctx->flags & FLAG_LEFT_JUSTIFY)
    {
        /* Print the string with trailing spaces */
        ctx->chars_printed += ctx->nputs(ctx->arg, p_str, string_len);
        while (pad_len--)
        {
            ctx->chars_printed += ctx->nputs(ctx->arg, space, 1);
        }
    }
    else
    {
        /* If right justified print leading spaces */
        while (pad_len--)
        {
            ctx->chars_printed += ctx->nputs(ctx->arg, space, 1);
        }
        /* Print the string */
        ctx->chars_printed += ctx->nputs(ctx->arg, p_str, string_len);
    }
    return 0;
}

static int print_num(struct printf_ctx *ctx)
{
    int num_len;
    unsigned long long ullval;
    int neg = 0;
    unsigned radix;
    const char *digit;
    char sign_char;
    int prepend_width;
    int padding_width;
    const char *padding_char;
    unsigned long long scale;

    /****************************************
     * Read value depending on the length
     * modifier and sign
     ***************************************/
    if (ctx->flags & FLAG_UNSIGNED)
    {
        switch (ctx->flags & FLAG_SIZE_MASK)
        {
            case FLAG_CHAR_T:
                ullval = (unsigned char)va_arg(ctx->ap, unsigned int);
                break;
            case FLAG_SHORT_T:
                ullval = (unsigned short)va_arg(ctx->ap, unsigned int);
                break;
            case FLAG_LONG_T:
                ullval = va_arg(ctx->ap, unsigned long);
                break;
            case FLAG_LONGLONG_T:
                ullval = va_arg(ctx->ap, unsigned long long);
                break;
            case FLAG_INTMAX_T:
                ullval = va_arg(ctx->ap, uintmax_t);
                break;
            case FLAG_SIZE_T:
                ullval = va_arg(ctx->ap, size_t);
                break;
            case FLAG_PTRDIFF_T:
                ullval = va_arg(ctx->ap, ptrdiff_t);
                break;
            default:
                ullval = va_arg(ctx->ap, unsigned int);
                break;
        }
    }
    else
    {
        long long llval;
        switch (ctx->flags & FLAG_SIZE_MASK)
        {
            case FLAG_CHAR_T:
                llval = (char)va_arg(ctx->ap, int);
                break;
            case FLAG_SHORT_T:
                llval = (short)va_arg(ctx->ap, int);
                break;
            case FLAG_LONG_T:
                llval = va_arg(ctx->ap, long);
                break;
            case FLAG_LONGLONG_T:
                llval = va_arg(ctx->ap, long long);
                break;
            case FLAG_INTMAX_T:
                llval = va_arg(ctx->ap, intmax_t);
                break;
            case FLAG_SIZE_T:
                llval = va_arg(ctx->ap, size_t);
                break;
            case FLAG_PTRDIFF_T:
                llval = va_arg(ctx->ap, ptrdiff_t);
                break;
            default:
                llval = va_arg(ctx->ap, int);
                break;
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
    if (ctx->flags & FLAG_HEX)
    {
        radix = 16;
    }
    else if (ctx->flags & FLAG_OCTAL)
    {
        radix = 8;
    }
    else
    {
        radix = 10;
    }

    /****************************************
     * The sign char
     ***************************************/
    sign_char = 0;
    if (neg)
    {
        sign_char = '-';
    }
    else
    {
        if (ctx->flags & FLAG_SHOW_SIGN)
        {
            sign_char = '+';
        }
        else if (ctx->flags & FLAG_SPACE)
        {
            sign_char = ' ';
        }
    }

    /****************************************
     * Work out how long the number will be
     ***************************************/
    scale = 1;
    num_len = 0;
    while (1)
    {
        unsigned long long new_scale;
        if (scale > ullval)
        {
            scale /= radix;
            break;
        }
        new_scale = scale * radix;
        ++num_len;
        if (new_scale < scale)
        {
            break;
        }
        scale = new_scale;
    }

    /****************************************
     * Was a precision supplied
     ***************************************/
    prepend_width = 0;

    if (ctx->precision != INT_MAX)
    {
        /* Spec. says that if a precision is specified, the 0 flag is
         * ignored. 0s will be used to pad out to the stated precision
         * but spaces to the width.
         */
        ctx->flags &= ~FLAG_ZEROPAD;
    }
    else
    {
        /* The default precision for numbers is 1. */
        ctx->precision = 1;
    }
    if (ctx->precision > num_len)
    {
        prepend_width = ctx->precision - num_len;
    }

    /****************************************
     * The alternative flag
     ***************************************/
//    if (flags & FLAG_ALTERNATIVE)
//    {
//        if (radix == 16)
//        {
//            prepend_width -= 2;
//            if (prepend_width < 0)
//            {
//                prepend_width = 0;
//            }
//        }
//        else if (radix == 8)
//        {
//            if (!prepend_width)
//            {
//                ++prepend_width;
//            }
//        }
//    }

    /****************************************
     * Pad to the width. Note that the ZEROPAD
     * flag is ignored if we are left-justifying
     ***************************************/
    padding_char = space;

    if ((ctx->flags & FLAG_ZEROPAD) && !(ctx->flags & FLAG_LEFT_JUSTIFY))
    {
        padding_char = zero;
    }

    padding_width = 0;
    if (ctx->field_width > (sign_char ? 1 : 0) + prepend_width + num_len)
    {
        padding_width = ctx->field_width - (sign_char ? 1 : 0) - prepend_width - num_len;
    }

    /****************************************
     * Print everything now
     ***************************************/
    if (!(ctx->flags & FLAG_LEFT_JUSTIFY))
    {
        while (padding_width--)
        {
            ctx->chars_printed += ctx->nputs(ctx->arg, padding_char, 1);
        }
    }

    if (sign_char)
    {
        ctx->chars_printed += ctx->nputs(ctx->arg, &sign_char, 1);
    }

//    if ((flags & FLAG_ALTERNATIVE) && (radix == 16))
//    {
//        char hex_prefix[] = "0x";
//        chars_printed += nputs(arg, hex_prefix, 2);
//    }

    while (prepend_width--)
    {
        ctx->chars_printed += ctx->nputs(ctx->arg, zero, 1);
    }

    /****************************************
     * Print the number
     ***************************************/
    digit = ctx->flags & FLAG_UPPER ? upper_digit : lower_digit;
    while (scale)
    {
        ctx->chars_printed += ctx->nputs(ctx->arg, &digit[ullval / scale], 1);
        ullval %= scale;
        scale /= radix;
    }

    /****************************************
     * Print padding on the right
     ***************************************/
    if (ctx->flags & FLAG_LEFT_JUSTIFY)
    {
        while (padding_width--)
        {
            ctx->chars_printed += ctx->nputs(ctx->arg, padding_char, 1);
        }
    }

    return 0;
}

static int debug_print_num(struct printf_ctx *ctx, unsigned long long val, int radix)
{
    char buf[50];
    char *p_num;
    int num_len;

    p_num = &buf[50];
    num_len = 0;
    while (val)
    {
        *--p_num = lower_digit[val % radix];
        val /= radix;
        ++num_len;
    }
    return ctx->nputs(ctx->arg, p_num, num_len);
}

extern char *_Anvil_dtoa(double dd, int mode, int ndigits, int *decpt, int *sign, char **rve);

static int print_double(struct printf_ctx *ctx)
{
    double dval = va_arg(ctx->ap, double);
    int decpt;
    int sign;
    char *str = _Anvil_dtoa(dval, 1, 6, &decpt, &sign, NULL);
    char *p = str;

    if (ctx->flags & FLAG_FIXED)
    {
        if (decpt > 0)
        {
            // If decpt is positive, we have to print decpt chars before the decimal
            // point
            for (int i=0; i<decpt; ++i)
            {
                if (*p)
                {
                    ctx->nputs(ctx->arg, p, 1);
                    ++p;
                }
                else
                {
                    ctx->nputs(ctx->arg, "0", 1);
                }
            }
            ctx->nputs(ctx->arg, ".", 1);
        }
        else
        {
            // If decpt is negative, we have to print decpt 0's after the decimal
            // point
            ctx->nputs(ctx->arg, "0", 1);
            ctx->nputs(ctx->arg, ".", 1);
            for (int i=0; i<-decpt; ++i)
            {
                ctx->nputs(ctx->arg, "0", 1);
            }
        }
        // Then print all the remaining chars, if any
        while (*p)
        {
            ctx->nputs(ctx->arg, p, 1);
            ++p;
        }
    }
    else if (ctx->flags & FLAG_EXP)
    {
        ctx->chars_printed += ctx->nputs(ctx->arg, "--------------------\n", 21);
        ctx->chars_printed += ctx->nputs(ctx->arg, p, strlen(p));
        ctx->chars_printed += ctx->nputs(ctx->arg, " ", 1);
        if (decpt < 0)
        {
            ctx->chars_printed += ctx->nputs(ctx->arg, "-", 1);
            debug_print_num(ctx, -decpt, 10);
        }
        else
        {
            debug_print_num(ctx, decpt, 10);
        }
        //ctx->chars_printed += ctx->nputs(ctx->arg, "\n", 1);
    }


    return 0;
}
