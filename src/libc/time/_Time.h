
#include <time.h>

static __inline__ int is_leap(int y)
{
    return  y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
}

static const char days_in_month_common_year[] =
{
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

static const int yday_of_first_of_month_common_year[] =
{
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
};

static __inline__ int days_in_month(int y, int m)
{
    return m != 1 || !is_leap(y) ? days_in_month_common_year[m] : 29;
}

static __inline__ int yday_of_first_of_month(int y, int m)
{
    return m <= 1 || !is_leap(y) ? yday_of_first_of_month_common_year[m] : yday_of_first_of_month_common_year[m] + 1;
}

void _Timet_to_tm(struct tm *tm, const time_t timer);
time_t _Tm_to_timet(const struct tm *timeptr);
