
#include "_Time.h"

time_t _Tm_to_timet(const struct tm *timeptr)
{
    time_t secs;
    int y, m, d;
    int era, yoe, doy, doe, days;

    /* For the date, we use the algorithms described in
     * http://howardhinnant.github.io/date_algorithms.html
     *
     *    m is in [1, 12]
     *    d is in [1, days_in_month(y, m)]
     *    y is the 0 based year
     *
     * We need to make sure that m and d are in range
     */
    y  = (timeptr->tm_mon < 0 ? timeptr->tm_mon - 11 : timeptr->tm_mon) / 12;
    m  = timeptr->tm_mon - (12 * y) + 1;
    d  = timeptr->tm_mday;

    y += timeptr->tm_year + 1900;
    y -= m <= 2;

    era  = (y >= 0 ? y : y - 399) / 400;
    yoe  = y - era * 400;                                    // [0, 399]
    doy  = ( 153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;  // [0, 365]
    doe  = yoe * 365 + yoe / 4 - yoe / 100 + doy;            // [0, 146096]
    days = era * 146097 + doe - 719468;

    secs  = timeptr->tm_sec;
    secs += timeptr->tm_min * 60;
    secs += timeptr->tm_hour * 3600;
    secs += days * 86400LL;

    return secs;
}
