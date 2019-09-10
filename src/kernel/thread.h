
#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

#include "arm.h"
#include "dlist.h"

#include <stddef.h>

enum
{
    THR_ST_RUNNING,
};

struct thread_obj
{
    dlist_chain_t chain;
    int id;
    int state;
    struct regpack *reg;
    char *stk;
    unsigned long psp;
    size_t stk_sz;
};

void thread_init();

#endif /* THREAD_H_INCLUDED */
