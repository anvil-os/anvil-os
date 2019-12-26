
#include <stdlib.h>

long int atol(const char *nptr)
{
    return strtol(nptr, (char **)NULL, 10);
}
