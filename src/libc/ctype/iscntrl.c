
#include <ctype.h>

int iscntrl(int c)
{
    _ANVIL_CTYPE_RANGE_FIX(c);
    return _C_Ctype[c] & _Anvil_CtCtl;
}
