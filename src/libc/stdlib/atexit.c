
#include "_Anvil_atexit.h"

int atexit(void (*func)(void))
{
    return _Anvil_atexit_reg(&_Anvil_atexit_list, func);
}
