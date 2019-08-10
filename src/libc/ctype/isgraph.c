
#include <ctype.h>

int isgraph(int c)
{
    _ANVIL_CTYPE_RANGE_FIX(c);
    return _C_Ctype[c] & (_Anvil_CtPnc | _Anvil_CtLow | _Anvil_CtUpp | _Anvil_CtNum);
}
