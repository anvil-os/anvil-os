
#include <ctype.h>

int isupper(int c)
{
    _ANVIL_CTYPE_RANGE_FIX(c);
    return _C_Ctype[c] & _Anvil_CtUpp;
}
