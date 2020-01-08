
#include "_Anvil_stdio.h"

#include <stdlib.h>

int setvbuf(FILE *restrict stream, char *restrict buf, int mode, size_t size)
{
    char *stream_buf = NULL;
    size_t stream_buf_size;
    int status = 0;

    switch (mode)
    {
        case _IONBF:
            stream_buf = stream->__minibuf;
            stream_buf_size = sizeof(stream->__minibuf);
            status |= _ANVIL_STDIO_NOBUF;
            break;

        case _IOFBF:
        case _IOLBF:
            if (buf)
            {
                // If a buffer is passed in we should use it
                stream_buf = buf;
                stream_buf_size = size;
            }
            else if (size)
            {
                // If it's size only use that
                stream_buf_size = size;
            }
            else
            {
                // If there is no size use the default size
                stream_buf_size = BUFSIZ;
            }
            if (stream_buf == NULL)
            {
                if ((stream_buf = (char *)malloc(stream_buf_size)) == NULL)
                {
                    return -1;
                }
                status |= _ANVIL_STDIO_BUF_MALLOCED;
            }
            status |= (mode == _IOLBF ? _ANVIL_STDIO_LINEBUF : _ANVIL_STDIO_FULLBUF);
            break;

        default :
            return -1;
    }

    if ( (stream->__status & (_ANVIL_STDIO_READING | _ANVIL_STDIO_WRITING)) || (stream->__buf) )
    {
        // We can't change the buffer if reading or writing are in progress
        // or if setvbuf has succeeded before
        return -1;
    }

    stream->__buf = stream_buf;
    stream->__buf_size = stream_buf_size;
    stream->__status &= ~(_ANVIL_STDIO_NOBUF | _ANVIL_STDIO_LINEBUF |_ANVIL_STDIO_FULLBUF | _ANVIL_STDIO_BUF_MALLOCED);
    stream->__status |= status;

    // Tail call _Anvil_setbuf to set all the other members of stream
    _Anvil_initbuf(stream);

    return 0;
}
