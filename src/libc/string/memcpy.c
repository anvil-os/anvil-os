
#include <string.h>

void *memcpy(void *restrict s1, const void *restrict s2, size_t n)
{
    char *p1 = s1;
    const char *p2 = s2;
    while (n--)
    {
        *p1++ = *p2++;
    }
    return s1;
}
