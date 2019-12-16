
#include <stdlib.h>

void *malloc(size_t size)
{
    return _Anvil_malloc(size);
    //return _Anvil_realloc(NULL, size);
}
