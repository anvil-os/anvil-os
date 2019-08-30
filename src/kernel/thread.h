
#ifndef _THREAD_H_INCLUDED
#define _THREAD_H_INCLUDED

#include "arm.h"

struct thread_obj
{
    int id;
    //struct regpack *reg;
    unsigned long *stk;
    unsigned long psp;
};

void thread_init();

#endif /* _THREAD_H_INCLUDED */
