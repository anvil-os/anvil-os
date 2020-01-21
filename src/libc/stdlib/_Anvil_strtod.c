
#include <stdlib.h>

#include <limits.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>

double ten_to_e(int e)
{
    double res = 1.0;
    if (e > 0)
    {
        for (int i=0; i<e; ++i)
        {
            res *= 10.0;
        }
    }
    else
    {
        e = -e;
        for (int i=0; i<e; ++i)
        {
            res /= 10.0;
        }
    }

    return res;
}

void dump_double(double z)
{
    union
    {
        double dbl;
        uint64_t uint;
    } z_z;
    z_z.dbl = z;

    uint32_t hi = (uint64_t)z_z.uint >> 32 & 0xffffffff;
    uint32_t lo = (uint64_t)z_z.uint & 0xffffffff;
    printf("%08x %08x\n", hi, lo);
}

double _Anvil_strtod(const char *restrict nptr, char **restrict endptr)
{
    const char *str = nptr;
    uint64_t mantissa;
    int exponent;
    int base;

    int neg;
    int exp_neg;
    int mantissa_exp;
    int dec_point;
    int mantissa_full;

    // Skip the whitespace
    while (isspace(*str))
    {
        ++str;
    }

    // Look for the sign next
    switch (*str)
    {
        case '-':
            ++str;
            neg = 1;
            break;
        case '+':
            ++str;
            /* Fall through */
        default:
            neg = 0;
            break;
    }

    // The base can be 10 or 16
    base = 10;
    if (*nptr == '0')
    {
        if (*(nptr+1) == 'x' || *(nptr+1) == 'X')
        {
            base = 16;
            nptr += 2;;
        }
        // Todo: What if the next char isn't a hex digit
    }

    mantissa = 0;
    mantissa_full = 0;
    dec_point = 0;
    mantissa_exp = 0;

    while (1)
    {
        int digit;
        if (*str <= '9' && *str >= '0')
        {
            digit = *str - '0';
        }
        else if (base == 16 && *nptr <= 'Z' && *nptr >= 'A')
        {
            digit = *nptr - 'A' + 10;
        }
        else if (base == 16 && *nptr <= 'z' && *nptr >= 'a')
        {
            digit = *nptr - 'a' + 10;
        }
        else if (*str == '.')
        {
            dec_point = 1;
            ++str;
            continue;
        }
        else
        {
            // Non-digit, we're done
            break;
        }

        if (!dec_point)
        {
            // We haven't seen the decimal yet
            if (!mantissa_full)
            {
                uint64_t new_mantissa = mantissa * base + digit;
                if (new_mantissa < mantissa)
                {
                    // We overflowed - record the error but keep eating digits
                    mantissa_full = 1;
                }
                else
                {
                    mantissa = new_mantissa;
                }
            }
            else
            {
                ++mantissa_exp;
            }
        }
        else
        {
            if (!mantissa_full)
            {
                mantissa = mantissa * base + digit;
                --mantissa_exp;
            }
            else
            {
            }
        }
        ++str;
    }

    exponent = 0;

    if (*str == 'e' || *str == 'E')
    {
        ++str;
        // Look for the sign first
        switch (*str)
        {
            case '-':
                ++str;
                exp_neg = 1;
                break;
            case '+':
                ++str;
                // Fall through
            default:
                exp_neg = 0;
                break;
        }

        while (1)
        {
            int digit;
            if (*str <= '9' && *str >= '0')
            {
                digit = *str - '0';
            }
            else
            {
                /* Non-digit, we're done */
                break;
            }

            int new_exponent = exponent * 10 + digit;
            if (new_exponent < exponent)
            {
                // We overflowed - record the error but keep eating digits
                mantissa_full = 1;
            }
            else
            {
                exponent = new_exponent;
            }
            ++str;
        }
        if (exp_neg)
        {
            exponent = -exponent;
        }
    }

    printf("m=%lld e=%d\n", mantissa, exponent + mantissa_exp);

    double estimate = (double)mantissa * ten_to_e(exponent + mantissa_exp);

    dump_double(estimate);

    if (neg)
    {
        estimate = -estimate;
    }
    return estimate;
}
