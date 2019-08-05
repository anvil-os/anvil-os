
#ifndef _STRING_H_INCLUDED
#define _STRING_H_INCLUDED

#define __need_NULL
#define __need_size_t
#include <stddef.h>

int memcmp(const void *__s1, const void *__s2, size_t __n);
void *memset(void *__s, int __c, size_t __n);
void *memcpy(void *restrict __s1, const void *restrict __s2, size_t __n);
void *memmove(void *__s1, const void *__s2, size_t __n);

#endif
