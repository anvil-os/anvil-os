
#include <stdlib.h>

void *realloc(void *ptr, size_t size)
{
    return _Anvil_realloc(ptr, size);
}
