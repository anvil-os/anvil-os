
#include <stdlib.h>

div_t div(int numer, int denom)
{
    div_t ret;
    ret.quot = numer / denom;
    ret.rem = numer % denom;
    return ret;
}
