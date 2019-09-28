
#ifndef _STDIO_H_INCLUDED
#define _STDIO_H_INCLUDED

#ifndef _SYS_ANVIL_INTERNAL_H_INCLUDED
#include <sys/anvil_internal.h>
#endif

#define __need_NULL
#define __need_size_t
#include <stddef.h>

#define _IOFBF          0    /* Fully buffered */
#define _IOLBF          1    /* Line buffered  */
#define _IONBF          2    /* Unbuffered     */
#define BUFSIZ          512

#define EOF             (-1)

#define FOPEN_MAX       32
#define FILENAME_MAX    1024
#define L_tmpnam        32

#define SEEK_SET        0    /* Seek relative to beginning of file     */
#define SEEK_CUR        1    /* Seek relative to current file position */
#define SEEK_END        2    /* Seek relative to end of file           */

#define TMP_MAX         10000

typedef struct
{
    int fd;
} FILE;

typedef struct
{
    _Anvil_off_t __pos;
} fpos_t;

FILE *stderr;
FILE *stdin;
FILE *stdout;

int remove(const char *__filename);
int rename(const char *__old, const char *__new);
FILE *tmpfile(void);
char *tmpnam(char *__s);
int fclose(FILE *__stream);
int fflush(FILE *__stream);
FILE *fopen(const char *restrict __filename, const char *restrict __mode);
FILE *freopen(const char *restrict __filename, const char *restrict __mode, FILE *restrict __stream);
void setbuf(FILE *restrict __stream, char *restrict __buf);
int setvbuf(FILE *restrict __stream, char *restrict __buf, int __mode, size_t __size);
int fprintf(FILE *restrict __stream, const char *restrict __format, ...);
int fscanf(FILE *restrict __stream, const char *restrict __format, ...);
int printf(const char *restrict __format, ...);
int scanf(const char *restrict __format, ...);
int snprintf(char *restrict __s, size_t __n, const char *restrict __format, ...);
int sprintf(char *restrict __s, const char *restrict __format, ...);
int sscanf(const char *restrict __s, const char *restrict __format, ...);
int vfprintf(FILE *restrict __stream, const char *restrict __format, _Anvil_va_list __arg);
int vfscanf(FILE *restrict __stream, const char *restrict __format, _Anvil_va_list __arg);
int vprintf(const char *restrict __format, _Anvil_va_list __arg);
int vscanf(const char *restrict __format, _Anvil_va_list __arg);
int vsnprintf(char *restrict __s, size_t __n, const char *restrict __format, _Anvil_va_list __arg);
int vsprintf(char *restrict __s, const char *restrict __format, _Anvil_va_list __arg);
int vsscanf(const char *restrict __s, const char *restrict __format, _Anvil_va_list __arg);
int fgetc(FILE *__stream);
char *fgets(char *restrict __s, int __n, FILE *restrict __stream);
int fputc(int __c, FILE *__stream);
int fputs(const char *restrict __s, FILE *restrict __stream);
int getc(FILE *__stream);
int getchar(void);
int putc(int __c, FILE *__stream);
int putchar(int __c);
int puts(const char *__s);
int ungetc(int __c, FILE *__stream);
size_t fread(void *restrict __ptr, size_t __size, size_t __nmemb, FILE *restrict __stream);
size_t fwrite(const void *restrict __ptr, size_t __size, size_t __nmemb, FILE *restrict __stream);
int fgetpos(FILE *restrict __stream, fpos_t *restrict __pos);
int fseek(FILE *__stream, long int __offset, int __whence);
int fsetpos(FILE *__stream, const fpos_t *__pos);
long int ftell(FILE *__stream);
void rewind(FILE *__stream);
void clearerr(FILE *__stream);
int feof(FILE *__stream);
int ferror(FILE *__stream);
void perror(const char *__s);

#endif /* _STDIO_H_INCLUDED */
