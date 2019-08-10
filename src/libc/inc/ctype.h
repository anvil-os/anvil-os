
#ifndef _CTYPE_H_INCLUDED
#define _CTYPE_H_INCLUDED

#ifndef _SYS_ANVIL_INTERNAL_H_INCLUDED
#include <sys/anvil_internal.h>
#endif

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

/* Todo: move these to the C locale */
extern const short _C_Ctype[257];
extern const unsigned char _C_toupper[257];
extern const unsigned char _C_tolower[257];

#endif /* _CTYPE_H_INCLUDED */
