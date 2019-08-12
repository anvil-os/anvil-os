
#include <stdlib.h>
#include <limits.h>

unsigned long strtoul(const char *restrict nptr, char **restrict endptr, int base)
{
    return (unsigned long)_Anvil_strtoll(nptr, endptr, base, 0, ULONG_MAX);
}
