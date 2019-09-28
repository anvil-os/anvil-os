
#ifndef _SYS_ANVIL_INTERNAL_H_INCLUDED
#define _SYS_ANVIL_INTERNAL_H_INCLUDED

/*
 * These are here to help Eclipse. Because __ANVIL__ is always defined these
 * will never be seen by the compiler
 */
#if ! defined(__ANVIL__)
#define __GNUC__            4
#define GCC_TM_PREDS_H

#define __INTPTR_TYPE__         int
#define __UINTPTR_TYPE__        unsigned int

#define __INTMAX_TYPE__         long long int
#define __UINTMAX_TYPE__        long long unsigned int

#define __SIZE_TYPE__           unsigned int

#define __INT_MAX__             2147483647
#define __WCHAR_MAX__           2147483647
#define __SHRT_MAX__            32767
#define __SCHAR_MAX__           127
#define __INT_MAX__             2147483647
#define __INTMAX_MAX__          9223372036854775807L
#define __SIZE_MAX__            18446744073709551615UL
#define __SIZEOF_SIZE_T__       8

#define __INT8_TYPE__           signed char
#define __INT16_TYPE__          short int
#define __INT32_TYPE__          long int
#define __INT64_TYPE__          long long int
#define __UINT8_TYPE__          unsigned char
#define __UINT16_TYPE__         short unsigned int
#define __UINT32_TYPE__         long unsigned int
#define __UINT64_TYPE__         long long unsigned int

#define __INT_LEAST8_TYPE__     signed char
#define __INT_LEAST16_TYPE__    short int
#define __INT_LEAST32_TYPE__    long int
#define __INT_LEAST64_TYPE__    long long int
#define __UINT_LEAST8_TYPE__    unsigned char
#define __UINT_LEAST16_TYPE__   short unsigned int
#define __UINT_LEAST32_TYPE__   long unsigned int
#define __UINT_LEAST64_TYPE__   long long unsigned int

#define __INT_FAST8_TYPE__      int
#define __INT_FAST16_TYPE__     int
#define __INT_FAST32_TYPE__     int
#define __INT_FAST64_TYPE__     long long int
#define __UINT_FAST8_TYPE__     unsigned int
#define __UINT_FAST16_TYPE__    unsigned int
#define __UINT_FAST32_TYPE__    unsigned int
#define __UINT_FAST64_TYPE__    long long unsigned int

#define restrict

#define __gnuc_va_list unsigned char *

#endif

#define _Anvil_CtCtl (0x01)
#define _Anvil_CtSpc (0x02)
#define _Anvil_CtPnc (0x04)
#define _Anvil_CtNum (0x08)
#define _Anvil_CtUpp (0x10)
#define _Anvil_CtHex (0x20)
#define _Anvil_CtLow (0x40)
#define _Anvil_CtBln (0x80)

/* The standard says the ctype functions need to accept ints in the range
 * 0 to 255 plus EOF. Let's assume EOF is -1. We will each char's info
 * at offset +1 from its value. i.e [0] to [256]
 * We also support signed chars being passed in as negative ints
 *
 *  -127..-2 ==> 129..255
 *  -1 (EOF) ==> 0
 *  0..255   ==> 1..256
 */
#define _ANVIL_CTYPE_RANGE_FIX(__c) do { c = (c == -1 ? 0 : (unsigned char)c + 1); } while (0);
//#define _ANVIL_CTYPE_RANGE_FIX(__c) do { ++c; } while (0);

long long _Anvil_strtoll(const char *restrict __str,
                    char **restrict __endptr,
                    int __base, long long __min,
                    unsigned long long __max);

void *_Anvil_realloc(void *__ptr, __SIZE_TYPE__ __size);

#define _Anvil_va_list  __builtin_va_list
#define _Anvil_int64_t  __INT64_TYPE__

#define _Anvil_off_t    __INT64_TYPE__

int _Anvil_printf(const char *__fmt, _Anvil_va_list __ap, int (*__nputs)(void *, const char *, int), void *__arg);

extern int _Errno;

#endif /* _SYS_ANVIL_INTERNAL_H_INCLUDED */
