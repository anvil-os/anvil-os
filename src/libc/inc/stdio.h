
#ifndef _STDIO_H_INCLUDED
#define _STDIO_H_INCLUDED

#ifndef _SYS_ANVIL_INTERNAL_H_INCLUDED
#include <sys/anvil_internal.h>
#endif

int printf(const char *restrict __format, ...);
int vprintf(const char *restrict __format, va_list __arg);

#endif /* _STDIO_H_INCLUDED */
