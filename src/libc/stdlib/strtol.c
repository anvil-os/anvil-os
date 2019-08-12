
#include <stdlib.h>
#include <limits.h>

long strtol(const char *restrict nptr, char **restrict endptr, int base)
{
    return (long)_Anvil_strtoll(nptr, endptr, base, LONG_MIN, LONG_MAX);
}
