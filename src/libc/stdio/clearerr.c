
#include "_Anvil_stdio.h"

void clearerr(FILE *stream)
{
    stream->__status &= ~(_ANVIL_STDIO_ERR | _ANVIL_STDIO_EOF);
}
