
#include <threads.h>
#include <sys/anvil.h>

int thrd_create(thrd_t *thr, thrd_start_t func, void *arg)
{
    int tid = anvil_threadcreate((void * (*)(void*))func, arg);
    if (tid < 0)
    {
        return thrd_error;
    }
    if (thr)
    {
        *thr = tid;
    }
    return 0;
}
