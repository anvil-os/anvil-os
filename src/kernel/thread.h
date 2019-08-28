
#ifndef _THREAD_H_INCLUDED
#define _THREAD_H_INCLUDED

#include "arm.h"

struct thread_obj
{
    int id;
    struct regpack *reg;
};

#endif /* _THREAD_H_INCLUDED */
