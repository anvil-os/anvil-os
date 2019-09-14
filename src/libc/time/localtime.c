
#include "_Time.h"

struct tm *localtime(const time_t *timer)
{
    static struct tm tm;
    _Timet_to_tm(&tm, *timer);
    /* Todo: Timezone here */
    return &tm;
}
