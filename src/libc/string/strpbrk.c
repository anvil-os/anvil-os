
#include <string.h>

char *strpbrk(const char *s1, const char *s2)
{
    const char *p = s1;
    while (*p)
    {
        if (strchr(s2, *p))
        {
            return (char *)p;
        }
        ++p;
    }
    return NULL;
}
