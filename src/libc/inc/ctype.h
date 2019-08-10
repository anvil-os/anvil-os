
#ifndef _CTYPE_H_INCLUDED
#define _CTYPE_H_INCLUDED

int isalnum(int __c);
int isalpha(int __c);
int isblank(int __c);
int iscntrl(int __c);
int isdigit(int __c);
int isgraph(int __c);
int islower(int __c);
int isprint(int __c);
int ispunct(int __c);
int isspace(int __c);
int isupper(int __c);
int isxdigit(int __c);

int tolower(int __c);
int toupper(int __c);

#define _Anvil_CtCtl (0x01)
#define _Anvil_CtSpc (0x02)
#define _Anvil_CtPnc (0x04)
#define _Anvil_CtNum (0x08)
#define _Anvil_CtUpp (0x10)
#define _Anvil_CtHex (0x20)
#define _Anvil_CtLow (0x40)
#define _Anvil_CtBln (0x80)

/* The standard says the ctype functions need to accept ints in the range
 * 0 to 255 plus EOF. Let's assume EOF is -1. We will store EOF info
 * at offset 256.
 * We also support signed chars being passed in as negative ints
 *
 *  -127..-2 ==> 128..254
 *  -1 (EOF) ==> 256
 *  0..255   ==> 0..255
 */
#define _ANVIL_CTYPE_RANGE_FIX(__c) do { c = c == -1 ? 256 : (unsigned char)c; } while (0);

extern const short _C_Ctype[257];
extern const short _C_toupper[257];
extern const short _C_tolower[257];

#endif /* _CTYPE_H_INCLUDED */
