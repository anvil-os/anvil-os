
#include <ctype.h>

int isdigit(int c)
{
    _ANVIL_CTYPE_RANGE_FIX(c);
    return _C_Ctype[c] & _Anvil_CtNum;
}
