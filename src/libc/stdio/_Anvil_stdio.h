
#include <stdio.h>

#define _ANVIL_STDIO_ERR            1
#define _ANVIL_STDIO_EOF            2

#define _ANVIL_STDIO_READABLE       4
#define _ANVIL_STDIO_WRITEABLE      8

#define _ANVIL_STDIO_BYTE           16
#define _ANVIL_STDIO_WIDE           32

#define _ANVIL_STDIO_READING        64
#define _ANVIL_STDIO_WRITING        128

#define _ANVIL_STDIO_NOBUF          256
#define _ANVIL_STDIO_LINEBUF        512
#define _ANVIL_STDIO_FULLBUF        1024

#define _ANVIL_STDIO_BUF_MALLOCED   2048
#define _ANVIL_STDIO_FILE_MALLOCED  4096

void _Anvil_flockfile(FILE *stream);
void _Anvil_funlockfile(FILE *stream);
void _Anvil_setbuf(FILE *restrict stream);
int _Anvil_fputc(int __c, FILE *__stream);
long _Anvil_write(int __fildes, const void *__buf, size_t __nbyte);
int _Anvil_fflush(FILE *__stream);
int _Anvil_open(const char *__path, int __oflag);
int _Anvil_close(int __fd);

// These should be in fcntl.h when we have one
#define O_RDONLY        1
#define O_WRONLY        2
#define O_RDWR          4
#define O_CREAT         8
#define O_TRUNC         16
#define O_APPEND        32
#define O_EXCL          64

#define _Anvil_fputc_unlocked(__c, __f) _Anvil_fputc((__c), (__f))
