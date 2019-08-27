
#ifndef _SYS_SYSCALL_FUNCS_H_INCLUDED
#define _SYS_SYSCALL_FUNCS_H_INCLUDED

#include <sys/anvil_internal.h>
#include <stdint.h>
#include <errno.h>

static inline int _Syscall2(long __n, long __p1, long __p2) {

    int ret = 0;
    __asm__ __volatile__ (
            "svc #0\n\t"
    );
    return ret;
}

#endif /* _SYS_SYSCALL_FUNCS_H_INCLUDED */
