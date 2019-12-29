
#include <stdlib.h>

long double strtold(const char *restrict nptr, char **restrict endptr)
{
    return _Anvil_strtold(nptr, endptr);
}
