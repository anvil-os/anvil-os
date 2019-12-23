
#include "_Anvil_stdio.h"

int feof(FILE *stream)
{
    return stream->__status & _ANVIL_STDIO_EOF;
}
