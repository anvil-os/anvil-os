
#include <errno.h>
#include <sys/syscalls.h>
#include <stdio.h>

#include "syscall.h"

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

extern struct thread_obj *pcurrt;

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

    pcurrt->psp = psp_get();

    struct regpack *reg = (struct regpack *)(pcurrt->psp);

    int syscall = reg->r0;

    printf("r0=%lx r1=%lx r2=%lx\n", reg->r0, reg->r1, reg->r2);

    /* Check that the syscall number is valid */
    if (syscall > __enum_kcall_nop)
    {
        RETVAL = -ENOSYS;
        return;
    }

    RETVAL = SysCall[syscall](pcurrt);

    psp_set(pcurrt->psp);
}
