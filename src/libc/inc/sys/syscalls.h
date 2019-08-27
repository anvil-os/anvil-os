
#ifndef _SYS_SYSCALLS_H_INCLUDED
#define _SYS_SYSCALLS_H_INCLUDED

/*
 * This creates a list of enums for the syscall handlers. This needs to
 * be included by the user-side system call functions so they match the
 * kernel expectation.
 */

#define SYSCALL(func)     __enum_ ## func,
enum
{
#include "_syscall_list.h"
};
#undef SYSCALL

#endif /* _SYS_SYSCALLS_H_INCLUDED */
