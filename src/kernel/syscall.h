
#ifndef _SYSCALL_H_INCLUDED
#define _SYSCALL_H_INCLUDED

#include "thread.h"

/* This creates a list of function prototypes for the syscall handlers */
#define SYSCALL(func) int func (struct thread_obj *t);
#include <sys/_syscall_list.h>
#undef SYSCALL

#define PARM1 (currt->reg->r0)
#define PARM2 (currt->reg->r1)
#define PARM3 (currt->reg->r2)
#define PARM4 (currt->reg->r3)
#define RETVAL (currt->reg->r0)

#endif /* _SYSCALL_H_INCLUDED */
