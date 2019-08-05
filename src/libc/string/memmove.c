
#include <string.h>

void *memmove(void *s1, const void *s2, size_t n)
{
    char *p1 = s1;
    const char *p2 = s2;

    /* There are 5 possibilities */
    if (p1 < p2)
    {
        if (p1 + n <= p2)
        {
            /* buf1 entirely before buf2 - doesn't matter */
            memcpy(p1, p2, n);
        }
        else
        {
            /* buf1 overlaps the start of buf2 - forward copy */
            while (n--)
            {
                *p1++ = *p2++;
            }
        }
    }
    else if (p2 < p1)
    {
        if (p2 + n <= p1)
        {
            /* buf1 entirely after buf2 - doesn't matter */
            memcpy(p1, p2, n);
        }
        else
        {
            /* buf1 overlaps the end of buf2 - reverse copy */
            p1 += n;
            p2 += n;
            while (n--)
            {
                *--p1 = *--p2;
            }
        }
    }
    else
    {
        /* Co-incident so no copy */
    }
    return s1;
}
