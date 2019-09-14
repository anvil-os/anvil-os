
#ifndef _TIME_H_INCLUDED
#define _TIME_H_INCLUDED

#ifndef _SYS_ANVIL_INTERNAL_H_INCLUDED
#include <sys/anvil_internal.h>
#endif

#define __need_NULL
#define __need_size_t
#include <stddef.h>

#define CLOCKS_PER_SEC      ((clock_t)1000000)
#define TIME_UTC            (1)

typedef int                 clock_t;
typedef long long           time_t;

struct timespec
{
    time_t tv_sec;  /* whole seconds -- ≥ 0                 */
    long tv_nsec;   /* nanoseconds -- [0, 999999999]        */
};

struct tm
{
    int tm_sec;     /* seconds after the minute -- [0, 60]  */
    int tm_min;     /* minutes after the hour -- [0, 59]    */
    int tm_hour;    /* hours since midnight -- [0, 23]      */
    int tm_mday;    /* day of the month -- [1, 31]          */
    int tm_mon;     /* months since January -- [0, 11]      */
    int tm_year;    /* years since 1900                     */
    int tm_wday;    /* days since Sunday -- [0, 6]          */
    int tm_yday;    /* days since January 1 -- [0, 365]     */
    int tm_isdst;   /* Daylight Saving Time flag            */
};

clock_t clock(void);
double difftime(time_t __time1, time_t __time0);
time_t mktime(struct tm *__timeptr);
time_t time(time_t *__timer);
int timespec_get(struct timespec *__ts, int __base);
char *asctime(const struct tm *__timeptr);
char *ctime(const time_t *__timer);
struct tm *gmtime(const time_t *__timer);
struct tm *localtime(const time_t *__timer);
size_t strftime(char *restrict __s, size_t __maxsize, const char *restrict __format, const struct tm *restrict __timeptr);

#endif /* _TIME_H_INCLUDED */
