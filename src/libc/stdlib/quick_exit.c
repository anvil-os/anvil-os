
#include "_Anvil_atexit.h"

struct _Anvil_atexit_record *_Anvil_at_quick_exit_list;

_Noreturn void quick_exit(int status)
{
    _Anvil_atexit_call(&_Anvil_at_quick_exit_list);
    _Exit(status);
}
