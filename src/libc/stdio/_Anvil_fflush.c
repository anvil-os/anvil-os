
#include "_Anvil_stdio.h"

int _Anvil_fflush(FILE *stream)
{
    if (stream->__status & _ANVIL_STDIO_WRITING)
    {
        // Writing is in progress so flush as a write buffer
        size_t remaining_chars = stream->__wptr - stream->__buf;
        char *buf = stream->__buf;

        // Write all the chars
        while (remaining_chars > 0)
        {
            long chars_written = _Anvil_write(stream->__fd, buf, remaining_chars);
            if (chars_written < 0)
            {
                // Todo: set error here
                return -1;
            }
            buf += chars_written;
            remaining_chars -= chars_written;
        }
    }

    _Anvil_initbuf(stream);
    stream->__status &= ~(_ANVIL_STDIO_WRITING | _ANVIL_STDIO_READING);

    return 0;
}
