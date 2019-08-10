
#include <ctype.h>

int isalnum(int c)
{
    _ANVIL_CTYPE_RANGE_FIX(c);
    return _C_Ctype[c] & (_Anvil_CtLow | _Anvil_CtUpp | _Anvil_CtNum);
}
