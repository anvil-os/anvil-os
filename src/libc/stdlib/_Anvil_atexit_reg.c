
#include "_Anvil_atexit.h"

int _Anvil_atexit_reg(struct _Anvil_atexit_record **list, void (*func)(void))
{
    struct _Anvil_atexit_record *a;
    if ((a = (struct _Anvil_atexit_record *)malloc(sizeof(*a))) == NULL)
    {
        return -1;
    }
    // Add to the head of the list
    a->func = func;
    a->pnext = *list;
    *list = a;
    return 0;
}
