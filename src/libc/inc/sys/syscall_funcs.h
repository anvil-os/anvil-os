
#ifndef _SYS_SYSCALL_FUNCS_H_INCLUDED
#define _SYS_SYSCALL_FUNCS_H_INCLUDED

#ifndef _SYS_ANVIL_INTERNAL_H_INCLUDED
#include <sys/anvil_internal.h>
#endif

#include <stdint.h>
#include <errno.h>

static inline int _Syscall2(long __n, long __p1, long __p2)
{
    register unsigned long ret __asm__ ("r0");
    register unsigned long n __asm__ ("r0") = __n;
    register unsigned long p1 __asm__ ("r1") = __p1;
    register unsigned long p2 __asm__ ("r2") = __p2;

    __asm__ __volatile__ (
            "svc #0\n\t"
            : "=r" (ret)
            : "r" (n), "r" (p1), "r" (p2)
            : "memory"
    );
    return ret;
}

#endif /* _SYS_SYSCALL_FUNCS_H_INCLUDED */
