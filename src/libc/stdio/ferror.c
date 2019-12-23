
#include "_Anvil_stdio.h"

int ferror(FILE *stream)
{
    return stream->__status & _ANVIL_STDIO_ERR;
}
