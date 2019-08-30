
#include "thread.h"
#include <stdio.h>

struct thread_obj thread_1;

struct thread_obj *pcurrt;

unsigned long stk1[256];

void thread_init()
{
    thread_1.id = 1;
    thread_1.stk = stk1;
    thread_1.psp = (long)(stk1 + 256);

    /* Point the psp at the thread 1 stack */
    psp_set(thread_1.psp);

    /* Point the msp at the kernel stack */
    msp_set(0x20002000);

    /* Set the thread mode stack to be the PSP */
    control_set(0x00000002);

    pcurrt = &thread_1;
}

int kcall_threadcreate(struct thread_obj *t)
{
    printf("kcall_threadcreate\n");
    return t->id;
}
