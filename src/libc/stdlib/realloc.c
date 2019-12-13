
#include <stdlib.h>

void *realloc(void *ptr, size_t size)
{
    if (size == 0)
    {
        // In the past it was assumed that if size were 0 then realloc would
        // simply free p and return NULL.
        // This is bad because a NULL return was supposed to indicate that the
        // ptr was not freed. We do nothing and indicate that by returning NULL
        return NULL;
    }
    return _Anvil_realloc(ptr, size);
}
