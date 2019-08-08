
#include <string.h>

char *strstr(const char *s1, const char *s2)
{
    size_t len_s2;
    if ((len_s2 = strlen(s2)) == 0)
    {
        return (char *)s1;
    }
    while (*s1)
    {
        if ((s1 = strchr(s1, *s2)) == NULL)
        {
            return NULL;
        }
        if (strncmp(s1, s2, len_s2) == 0)
        {
            return (char *)s1;
        }
        ++s1;
    }
    return NULL;
}
