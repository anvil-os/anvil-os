
#include "_Anvil_stdio.h"

#include <stdlib.h>

void _Anvil_initbuf(FILE *restrict stream)
{
    // This function must be called before any buffered I/O takes place. It
    // is therefore called in the long path of each of the read and write
    // functions. It can't be called from fopen because we need to delay
    // initialising the buffer in case the user calls setvbuf.
    // It's also called from ungetc and fflush.
    if (stream->__buf == NULL)
    {
        // There's no buffer yet so create a default one
        if ((stream->__buf = (char *)malloc(BUFSIZ)) != NULL)
        {
            // When opened, a stream is fully buffered if and only if it can be
            // determined not to refer to an interactive device.
            // The standard error stream is not fully buffered; the standard
            // input and standard output streams are fully buffered if and only
            // if the stream can be determined not to refer to an interactive
            // device.
            if (0/* stream is interactive */)
            {
                // It's interactive so we line buffer
                stream->__status |= _ANVIL_STDIO_LINEBUF;
            }
            else
            {
                // It's not interactive so we fully buffer
                stream->__status |= _ANVIL_STDIO_FULLBUF;
            }
            stream->__buf_size = BUFSIZ;
            stream->__status |= _ANVIL_STDIO_BUF_MALLOCED;
        }
        else
        {
            // No memory - so use the buffer in the FILE struct
            stream->__status |= _ANVIL_STDIO_NOBUF;
            stream->__buf = stream->__minibuf;
            stream->__buf_size = sizeof(stream->__minibuf);
        }
    }

    // Set all the ptrs to __bufend to force all read and writes macros to
    // call the appropriate functions
    // Todo: add other initialisation here
    stream->__bufend = stream->__buf + stream->__buf_size;
    stream->__rptr = stream->__bufend;
    stream->__wptr = stream->__bufend;
}
