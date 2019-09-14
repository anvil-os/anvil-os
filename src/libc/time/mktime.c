
#include "_Time.h"

time_t mktime(struct tm *timeptr)
{
    time_t secs;
    secs = _Tm_to_timet(timeptr);
    _Timet_to_tm(timeptr, secs);
    /* Todo: Timezone here */
    return secs;
}
