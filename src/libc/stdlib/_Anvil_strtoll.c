
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>

unsigned long long _Anvil_strtoll(
                    const char *restrict nptr,
                    char **restrict endptr,
                    int base,
                    long long min,
                    unsigned long long max)
{
    unsigned long long total;
    const char *restrict saved_nptr;
    int range_error;
    int neg;

    /* Set this now just in case we leave early */
    if (endptr)
    {
        *endptr = (char *)nptr;
    }

    /* Check that base is valid */
    if (base > 35 || base == 1 || base < 0)
    {
        errno = EINVAL;
        return 0;
    }

    /* Skip the whitespace */
    while (isspace(*nptr))
    {
        ++nptr;
    }

    /* Look for the sign next */
    switch (*nptr)
    {
        case '-':
            ++nptr;
            neg = 1;
            break;
        case '+':
            ++nptr;
            /* Fall through */
        default:
            neg = 0;
            break;
    }

    /* Calculate the max absolute value we can pick up */
    if (neg && (min < 0))
    {
        /* Signed and negative so max is abs(min) */
        max = -min;
    }

    switch (base)
    {
        case 0:
            /* If the base is 0 we need to work it out ourselves */
            base = 10;
            if (*nptr == '0')
            {
                base = 8;
                /* Don't move nptr here - let the loop below consume it */
                if (*(nptr+1) == 'x' || *(nptr+1) == 'X')
                {
                    base = 16;
                    nptr += 2;;
                }
            }
            break;
        case 16:
            /* If the base is 16 we allow an 0x on the front */
            if (*nptr == '0' && tolower(*(nptr+1)) == 'x')
            {
                nptr += 2;
            }
            break;
        default:
            break;
    }

    total = 0;
    range_error = 0;
    saved_nptr = nptr;

    while (1)
    {
        unsigned digit;
        if (*nptr <= '9' && *nptr >= '0')
        {
            digit = *nptr - '0';
        }
        else if (*nptr <= 'Z' && *nptr >= 'A')
        {
            digit = *nptr - 'A' + 10;
        }
        else if (*nptr <= 'z' && *nptr >= 'a')
        {
            digit = *nptr - 'a' + 10;
        }
        else
        {
            /* Non-digit, we're done */
            break;
        }
        if (digit >= (unsigned)base)
        {
            /* Out of range digit, we're done */
            break;
        }

        if (!range_error)
        {
            unsigned long long new_total = total * base + digit;
            if (new_total > max || new_total < total)
            {
                /* We overflowed - record the error but keep eating digits */
                total = max;
                range_error = 1;
                errno = ERANGE;
            }
            else
            {
                total = new_total;
            }
        }

        ++nptr;
    }

    if (saved_nptr == nptr)
    {
        /* We didn't find any useful chars */
        errno = EINVAL;
        return 0;
    }

    if (endptr)
    {
        *endptr = (char *)nptr;
    }

    if (neg)
    {
        total = -total;
    }

    return total;
}
