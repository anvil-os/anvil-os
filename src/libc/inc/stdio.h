
#ifndef _STDIO_H_INCLUDED
#define _STDIO_H_INCLUDED

#ifndef _SYS_ANVIL_INTERNAL_H_INCLUDED
#include <sys/anvil_internal.h>
#endif

typedef struct
{
    int fd;
} FILE;

#include <stddef.h>

int printf(const char *restrict __format, ...);
int vprintf(const char *restrict __format, va_list __arg);

int fprintf(FILE *restrict __stream, const char *restrict __format, ...);
int vfprintf(FILE *restrict __stream, const char *restrict __format, va_list __arg);

int sprintf(char *restrict __s, const char *restrict __format, ...);
int vsprintf(char *restrict __s, const char *restrict __format, va_list __arg);

int snprintf(char *restrict __s, size_t __n, const char *restrict __format, ...);
int vsnprintf(char *restrict __s, size_t __n, const char *restrict __format, va_list __arg);

int puts(const char *__s);

#endif /* _STDIO_H_INCLUDED */
