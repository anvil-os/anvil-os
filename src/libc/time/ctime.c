
#include "_Time.h"

char *ctime(const time_t *timer)
{
    return asctime(localtime(timer));
}
