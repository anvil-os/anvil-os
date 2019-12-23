
#ifndef _UCHAR_H_INCLUDED
#define _UCHAR_H_INCLUDED

#ifndef _SYS_ANVIL_INTERNAL_H_INCLUDED
#include <sys/anvil_internal.h>
#endif

#define __need_size_t
#include <stddef.h>

#if !defined (_Anvil_got_mbstate_t)
typedef _Anvil_mbstate_t mbstate_t;
#define _Anvil_got_mbstate_t
#endif

typedef __INT_LEAST16_TYPE__ char16_t;
typedef __INT_LEAST32_TYPE__ char32_t;

size_t mbrtoc16(char16_t *restrict __pc16, const char *restrict __s, size_t __n, mbstate_t *restrict __ps);
size_t c16rtomb(char *restrict __s, char16_t __c16, mbstate_t *restrict __ps);
size_t mbrtoc32(char32_t *restrict __pc32, const char *restrict __s, size_t __n, mbstate_t *restrict __ps);
size_t c32rtomb(char *restrict __s, char32_t __c32, mbstate_t *restrict __ps);

#endif /* _UCHAR_H_INCLUDED */
