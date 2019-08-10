
#include <ctype.h>

int isxdigit(int c)
{
    _ANVIL_CTYPE_RANGE_FIX(c);
    return _C_Ctype[c] & (_Anvil_CtHex | _Anvil_CtNum);
}
