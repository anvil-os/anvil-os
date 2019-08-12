
#include <stdlib.h>
#include <limits.h>

unsigned long long strtoull(const char *restrict nptr, char **restrict endptr, int base)
{
    return (unsigned long long)_Anvil_strtoll(nptr, endptr, base, 0, ULLONG_MAX);
}
