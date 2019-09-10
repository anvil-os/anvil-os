
#include "syscall.h"
#include "sched.h"

#include <errno.h>
#include <sys/syscalls.h>
#include <stdio.h>

/*
 * This creates an array of function pointers to the syscall handlers
 */
#define SYSCALL(func) func,
int (*(SysCall[]))(struct thread_obj *t) =
{
#include <sys/_syscall_list.h>
};
#undef SYSCALL

#define SYSCALL(func)     #func,
char *syscall_name[] =
{
#include <sys/_syscall_list.h>
};
#undef SYSCALL

int kcall_nop(struct thread_obj *t)
{
    printf("kcall_nop %p", (void *)t);
    return 0;
}

void syscall()
{
    /*
     * Syscalls use the std ARM abi to pass parameters
     *
     * parm   : r0 - the system call number
     * parm 1 : r1
     * parm 2 : r2
     * parm 3 : r3
     *
     * Additionally r0 holds the return value.
     */

    struct thread_obj *currt = sched_get_currt();

    currt->psp = psp_get();
    currt->reg = (struct regpack *)(currt->psp);

    int syscall = PARM1;

    printf("r0=%lx r1=%lx r2=%lx\n", PARM1, PARM2, PARM3);

    /* Check that the syscall number is valid */
    if (syscall > __enum_kcall_nop)
    {
        RETVAL = -ENOSYS;
        return;
    }

    RETVAL = SysCall[syscall](currt);

    //psp_set(pcurrt->psp);
}
