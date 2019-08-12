
#include <stdlib.h>
#include <limits.h>

long long strtoll(const char *restrict nptr, char **restrict endptr, int base)
{
    return (long long)_Anvil_strtoll(nptr, endptr, base, LLONG_MIN, LLONG_MAX);
}
