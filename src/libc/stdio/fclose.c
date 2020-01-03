
#include "_Anvil_stdio.h"
#include <stdlib.h>

int fclose(FILE *stream)
{
    _Anvil_close(stream->__fd);
    if (stream->__status & _ANVIL_STDIO_BUF_MALLOCED)
    {
        free(stream->__buf);
    }
    if (stream->__status & _ANVIL_STDIO_FILE_MALLOCED)
    {
        free(stream);
    }
    return 0;
}
