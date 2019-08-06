
#include <string.h>

char *strcpy(char *restrict s1, const char *restrict s2)
{
    char *restrict sret = s1;
    while ((*s1++ = *s2++))
    {
    }
    return sret;
}
