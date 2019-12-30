
#include "_Anvil_atexit.h"

int at_quick_exit(void (*func)(void))
{
    return _Anvil_atexit_reg(&_Anvil_at_quick_exit_list, func);
}
