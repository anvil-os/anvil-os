
#include <stdlib.h>

lldiv_t lldiv(long long int numer, long long int denom)
{
    lldiv_t ret;
    ret.quot = numer / denom;
    ret.rem = numer % denom;
    return ret;
}
