
#include <sys/anvil.h>
#include <sys/syscall_funcs.h>
#include <sys/syscalls.h>

int anvil_threadcreate(void *(*start_routine)(void*), void *arg)
{
    return _Syscall2(__enum_kcall_threadcreate, (long)start_routine, (long)arg);
}
