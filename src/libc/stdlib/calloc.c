
#include <stdlib.h>
#include <string.h>

void *calloc(size_t nelem, size_t elsize)
{
    size_t size;
    void *pmem;

    /* Check for overflow */
    size = nelem * elsize;
    if (nelem && (size / nelem != elsize))
    {
        return NULL;
    }

    if ((pmem = malloc(size)) == NULL)
    {
        /* Must be OOM or perhaps too big */
        return NULL;
    }

    memset(pmem, 0, size);

    return pmem;
}
