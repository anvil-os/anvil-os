
#include <stdlib.h>

ldiv_t ldiv(long int numer, long int denom)
{
    ldiv_t ret;
    ret.quot = numer / denom;
    ret.rem = numer % denom;
    return ret;
}
