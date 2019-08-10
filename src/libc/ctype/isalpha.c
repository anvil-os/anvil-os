
#include <ctype.h>

int isalpha(int c)
{
    _ANVIL_CTYPE_RANGE_FIX(c);
    return _C_Ctype[c] & (_Anvil_CtLow | _Anvil_CtUpp);
}
