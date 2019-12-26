
#include <stdio.h>

#define _ANVIL_STDIO_ERR        1
#define _ANVIL_STDIO_EOF        2

#define _ANVIL_STDIO_READABLE   4
#define _ANVIL_STDIO_WRITEABLE  8

#define _ANVIL_STDIO_BYTE       16
#define _ANVIL_STDIO_WIDE       32

#define _ANVIL_STDIO_READING    64
#define _ANVIL_STDIO_WRITING    128

#define _ANVIL_STDIO_NOBUF       256
#define _ANVIL_STDIO_LINEBUF     512
#define _ANVIL_STDIO_FULLBUF     1024

void _Anvil_flockfile(FILE *stream);
void _Anvil_funlockfile(FILE *stream);
void _Anvil_setbuf(FILE *stream);
int _Anvil_fputc(int __c, FILE *__stream);
long _Anvil_write(int fildes, const void *buf, size_t nbyte);
