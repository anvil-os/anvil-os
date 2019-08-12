
#ifndef _SYS_ANVIL_INTERNAL_H_INCLUDED
#define _SYS_ANVIL_INTERNAL_H_INCLUDED

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

unsigned long long _Anvil_strtoll(const char *restrict str,
                    char **restrict endptr,
                    int base, long long min,
                    unsigned long long max);

extern int _Errno;

#endif /* _SYS_ANVIL_INTERNAL_H_INCLUDED */
