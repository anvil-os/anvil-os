
#include <string.h>

char *strncat(char *restrict s1, const char *restrict s2, size_t n)
{
    /* We may end up with max strlen(s1)+n+1 chars */
    char *restrict sret = s1;
    while (*s1)
    {
        ++s1;
    }
    while (n-- && *s2)
    {
        *s1++ = *s2++;
    }
    *s1 = 0;
    return sret;
}
