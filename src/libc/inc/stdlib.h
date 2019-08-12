
#ifndef _STDLIB_H_INCLUDED
#define _STDLIB_H_INCLUDED

#ifndef _SYS_ANVIL_INTERNAL_H_INCLUDED
#include <sys/anvil_internal.h>
#endif

long int strtol(const char *restrict __nptr, char **restrict __endptr, int __base);
long long int strtoll(const char *restrict __nptr, char **restrict __endptr, int __base);
unsigned long int strtoul(const char *restrict __nptr, char **restrict __endptr, int __base);
unsigned long long int strtoull(const char *restrict __nptr, char **restrict __endptr, int __base);

#endif /* _STDLIB_H_INCLUDED */
