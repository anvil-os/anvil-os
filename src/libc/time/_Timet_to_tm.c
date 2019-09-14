
#include "_Time.h"

static const int secs_per_day = 24 * 60 * 60;

void _Timet_to_tm(struct tm *tm, const time_t timer)
{
    int day_num, secs;
    int era, doe, yoe, year, doy, mp, mday, mon;

    /* Split the time into days and seconds */
    day_num = timer / secs_per_day;
    secs    = timer % secs_per_day;

    /* For the date, we use the algorithms described in
     * http://howardhinnant.github.io/date_algorithms.html
     */
    day_num += 719468;
    era   = (day_num >= 0 ? day_num : day_num - 146096) / 146097;
    doe   = day_num - era * 146097;                                  /* [0, 146096]  */
    yoe   = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;   /* [0, 399]     */
    year  = yoe + era * 400;
    doy   = doe - (365 * yoe + yoe / 4 - yoe / 100);                 /* [0, 365]     */
    mp    = (5 * doy + 2) / 153;                                     /* [0, 11]      */
    mday  = doy - (153 * mp + 2) / 5 + 1;                            /* [1, 31]      */
    mon   = mp + (mp < 10 ? 3 : -9);                                 /* [1, 12]      */
    year += (mon <= 2);

    /* years since 1900 */
    tm->tm_year = year - 1900;
    /* months since January -- [0, 11]  */
    tm->tm_mon  = mon - 1;
    /* day of the month -- [1, 31]      */
    tm->tm_mday = mday;

    /* days since Sunday -- [0, 6] -  March 1, 0000 was a Wednesday so 3 */
    tm->tm_wday = day_num >= -3 ? (day_num + 3) % 7 : (day_num + 4) % 7 + 6;
    /* days since January 1 -- [0, 365]     */
    tm->tm_yday = yday_of_first_of_month(year, mon - 1) + mday - 1;
    /* Daylight Saving Time flag            */
    tm->tm_isdst = 0;

    /* Finally calculate the time */
    tm->tm_hour = secs / 3600;       /* hours since midnight -- [0, 23]      */
    secs       %= 3600;
    tm->tm_min  = secs / 60;         /* minutes after the hour -- [0, 59]    */
    secs       %= 60;
    tm->tm_sec  = secs;              /* seconds after the minute -- [0, 60]  */
}
