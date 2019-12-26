
#ifndef _STDLIB_H_INCLUDED
#define _STDLIB_H_INCLUDED

#ifndef _SYS_ANVIL_INTERNAL_H_INCLUDED
#include <sys/anvil_internal.h>
#endif

#define __need_NULL
#define __need_size_t
#define __need_wchar_t
#include <stddef.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define RAND_MAX 32767

#define MB_CUR_MAX 8

typedef struct
{
    int quot;
    int rem;
} div_t;

typedef struct
{
    long int quot;
    long int rem;
} ldiv_t;

typedef struct
{
    long long int quot;
    long long int rem;
} lldiv_t;

double atof(const char *__nptr);
int atoi(const char *__nptr);
long int atol(const char *__nptr);
long long int atoll(const char *__nptr);
double strtod(const char *restrict __nptr, char **restrict __endptr);
float strtof(const char *restrict __nptr, char **restrict __endptr);
long double strtold(const char *restrict __nptr, char **restrict __endptr);
long int strtol(const char *restrict __nptr, char **restrict __endptr, int __base);
long long int strtoll(const char *restrict __nptr, char **restrict __endptr, int __base);
unsigned long int strtoul(const char *restrict __nptr, char **restrict __endptr, int __base);
unsigned long long int strtoull(const char *restrict __nptr, char **restrict __endptr, int __base);
int rand(void);
void srand(unsigned int __seed);
void *aligned_alloc(size_t __alignment, size_t __size);
void *calloc(size_t __nmemb, size_t __size);
void free(void *__ptr);
void *malloc(size_t __size);
void *realloc(void *__ptr, size_t __size);
_Noreturn void abort(void);
int atexit(void (*func)(void));
int at_quick_exit(void (*__func)(void));
_Noreturn void exit(int __status);
_Noreturn void _Exit(int __status);
char *getenv(const char *__name);
_Noreturn void quick_exit(int __status);
int system(const char *__string);
void *bsearch(const void *__key, const void *__base, size_t __nmemb, size_t __size, int (*__compar)(const void *, const void *));
void qsort(void *__base, size_t __nmemb, size_t __size, int (*__compar)(const void *, const void *));
int abs(int __j);
long int labs(long int __j);
long long int llabs(long long int __j);
div_t div(int __numer, int __denom);
ldiv_t ldiv(long int __numer, long int __denom);
lldiv_t lldiv(long long int __numer, long long int __denom);
int mblen(const char *__s, size_t __n);
int mbtowc(wchar_t *restrict __pwc, const char *restrict __s, size_t __n);
int wctomb(char *__s, wchar_t __wchar);
size_t mbstowcs(wchar_t *restrict __pwcs, const char *restrict __s, size_t __n);
size_t wcstombs(char *restrict __s, const wchar_t *restrict __pwcs, size_t __n);

#endif /* _STDLIB_H_INCLUDED */
