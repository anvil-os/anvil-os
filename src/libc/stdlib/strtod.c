
#include <stdlib.h>

double strtod(const char *restrict nptr, char **restrict endptr)
{
    return _Anvil_strtod(nptr, endptr);
}
