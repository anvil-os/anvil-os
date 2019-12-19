
#ifndef _STDLIB_H_INCLUDED
#define _STDLIB_H_INCLUDED

#ifndef _SYS_ANVIL_INTERNAL_H_INCLUDED
#include <sys/anvil_internal.h>
#endif

#define __need_NULL
#define __need_size_t
#include <stddef.h>

long int strtol(const char *restrict __nptr, char **restrict __endptr, int __base);
long long int strtoll(const char *restrict __nptr, char **restrict __endptr, int __base);
unsigned long int strtoul(const char *restrict __nptr, char **restrict __endptr, int __base);
unsigned long long int strtoull(const char *restrict __nptr, char **restrict __endptr, int __base);

void *calloc(size_t __nmemb, size_t __size);
void free(void *__ptr);
void *malloc(size_t __size);
void *realloc(void *__ptr, size_t __size);

void abort(void);
int atexit(void (*func)(void));
char *getenv(const char *name);

int rand(void);
void srand(unsigned int __seed);

#endif /* _STDLIB_H_INCLUDED */
