
#include <ctype.h>

int isspace(int c)
{
    _ANVIL_CTYPE_RANGE_FIX(c);
    return _C_Ctype[c] & _Anvil_CtSpc;
}
