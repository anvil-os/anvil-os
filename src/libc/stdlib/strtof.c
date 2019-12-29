
#include <stdlib.h>

float strtof(const char *restrict nptr, char **restrict endptr)
{
    return (float)_Anvil_strtold(nptr, endptr);
}
