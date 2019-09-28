
#include "thread.h"
#include "syscall.h"
#include "sched.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>

struct thread_obj thread_1;

uint64_t stk1[64];

void thread_init()
{
    thread_1.id = 1;
    thread_1.stk_sz = 512;
    thread_1.stk = stk1;
    thread_1.psp = (uint32_t)(thread_1.stk + 64);

    /* Point the psp at the thread 1 stack */
    psp_set((uint32_t)thread_1.psp);
}

int kcall_threadcreate(struct thread_obj *currt)
{
    printf("kcall_threadcreate\n");

    struct thread_obj *p_thr;
    p_thr = (struct thread_obj *)malloc(sizeof (struct thread_obj));
    p_thr->stk_sz = 256;
    p_thr->stk = (uint64_t *)malloc(p_thr->stk_sz);
    p_thr->id = 2;
    p_thr->psp = (uint32_t)(p_thr->stk + p_thr->stk_sz - sizeof(struct regpack));

    struct regpack *preg = (struct regpack *)(p_thr->stk + p_thr->stk_sz - sizeof(struct regpack));
    preg->psr = 0x01000000;
    preg->pc = (unsigned long)PARM2;

    sched_add(p_thr, 0);

    return 0;
}
