
#ifndef _STRING_H_INCLUDED
#define _STRING_H_INCLUDED

#ifndef _SYS_ANVIL_INTERNAL_H_INCLUDED
#include <sys/anvil_internal.h>
#endif

#define __need_NULL
#define __need_size_t
#include <stddef.h>

int memcmp(const void *__s1, const void *__s2, size_t __n);
void *memset(void *__s, int __c, size_t __n);
void *memcpy(void *restrict __s1, const void *restrict __s2, size_t __n);
void *memmove(void *__s1, const void *__s2, size_t __n);
void *memchr(const void *__s, int __c, size_t __n);

size_t strlen(const char *__s);

char *strcpy(char *restrict __s1, const char *restrict __s2);
char *strncpy(char *restrict __s1, const char *restrict __s2, size_t __n);
char *strcat(char *restrict __s1, const char *restrict __s2);
char *strncat(char *restrict __s1, const char *restrict __s2, size_t __n);

int strcmp(const char *__s1, const char *__s2);
int strncmp(const char *__s1, const char *__s2, size_t __n);

char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);

size_t strspn(const char *__s1, const char *__s2);
size_t strcspn(const char *__s1, const char *__s2);
char *strpbrk(const char *__s1, const char *__s2);

char *strstr(const char *__s1, const char *__s2);
char *strtok(char *restrict __s1, const char *restrict __s2);

#endif
