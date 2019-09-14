
#include "_Time.h"

struct tm *gmtime(const time_t *timer)
{
    static struct tm tm;
    _Timet_to_tm(&tm, *timer);
    return &tm;
}
