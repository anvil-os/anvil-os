
#include <ctype.h>

int tolower(int c)
{
    _ANVIL_CTYPE_RANGE_FIX(c);
    return _C_tolower[c] ? _C_tolower[c] : c-1;
}
