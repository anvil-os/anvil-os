
#include <string.h>

char *strncpy(char *restrict s1, const char *restrict s2, size_t n)
{
    char *sret = s1;
    while (n)
    {
        --n;
        if (!(*s1++ = *s2++))
        {
            break;
        }
    }
    while (n--)
    {
        *s1++ = 0;
    }
    return sret;
}
