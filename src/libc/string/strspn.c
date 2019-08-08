
#include <string.h>

size_t strspn(const char *s1, const char *s2)
{
    const char *p = s1;
    while (*p)
    {
        if (!strchr(s2, *p))
        {
            break;
        }
        ++p;
    }
    return p - s1;
}
