
#include <ctype.h>

int toupper(int c)
{
    _ANVIL_CTYPE_RANGE_FIX(c);
    return _C_toupper[c] ? _C_toupper[c] : c-1;
}
