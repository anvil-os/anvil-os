
#include "_Anvil_atexit.h"

struct _Anvil_atexit_record *_Anvil_atexit_list;

_Noreturn void exit(int status)
{
    _Anvil_atexit_call(&_Anvil_atexit_list);
    _Exit(status);
}
