
#ifndef _SYS_ANVIL_H_INCLUDED
#define _SYS_ANVIL_H_INCLUDED

#ifndef _SYS_ANVIL_INTERNAL_H_INCLUDED
#include <sys/anvil_internal.h>
#endif

int anvil_threadcreate(int (*__start_routine)(void*), void *__arg);

#endif /* _SYS_ANVIL_H_INCLUDED */
