
#include <stdlib.h>

double strtod(const char *restrict nptr, char **restrict endptr)
{
    return (double)_Anvil_strtold(nptr, endptr);
}