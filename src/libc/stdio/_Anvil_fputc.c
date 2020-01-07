
#include "_Anvil_stdio.h"

int _Anvil_fputc(int c, FILE *stream)
{
    // The buffer below can hold 4 entries so when a write fills the buffer
    // (i.e. it will contain 4 entries) we will flush the buffer immediately
    //
    // This ensures that we never leave (or enter) this function with a full
    // buffer
    //
    //  |-----|-----|-----|-----|
    //  |  T  |  h  |     |     |
    //  |-----|-----|-----|-----|
    //     ^           ^     ^
    //     |           |     |
    //     |           |     |--- __bufend
    //     |           |
    //     |           |--------- __wptr - the next char will be written here
    //     |
    //     |--------------------- __buf
    //
    //
    // Unbuffered streams have a single byte buffer and the __buf, __bufend
    // and __wptr pointers will all point to that byte. This ensures a flush
    // on every byte written

    // If there is space in the buffer we've been through here and done all
    // the checks so store the char and return
    if (stream->__wptr < stream->__bufend)
    {
        *stream->__wptr++ = c;
        return c;
    }

    // Check that we are allowed to write -
    // a) must be writable (or read/writable)
    // b) must not be reading
    // c) must not be wide
    // d) must not be error
    if ( !(stream->__status & (_ANVIL_STDIO_WRITEABLE))  ||
          (stream->__status & (_ANVIL_STDIO_READING | _ANVIL_STDIO_WIDE | _ANVIL_STDIO_ERR)))
    {
        stream->__status |= _ANVIL_STDIO_ERR;
        return EOF;
    }

    // Commit to being a byte stream in write mode
    stream->__status |= (_ANVIL_STDIO_BYTE | _ANVIL_STDIO_WRITING);

    // If there is not a buffer yet, create one. Also initialises the pointers in the
    // FILE structure
    if (stream->__buf == NULL)
    {
        // This function guarantees a buffer of some type
        _Anvil_initbuf(stream);
    }

    if (stream->__wptr == stream->__buf + stream->__buf_size)
    {
        // _Anvil_initbuf leaves the stream in an initial state so we need to
        // restore writing mode with an empty buffer. This code is also executed
        // right after a flush which also returns all the buffers to their
        // initial states
        stream->__wptr = stream->__buf;
        stream->__bufend = stream->__buf + stream->__buf_size - 1;
    }

    // If we get here we have a buffer with room for at least one byte so put
    // it in the buffer now
    *stream->__wptr++ = c;

    // Flush the buffer if
    // a) it's full
    // b) it's line buffered and c is a \n
    // c) it's unbuffered - no special check needed for this since __wptr
    //    will equal __bufend + 1
    if ((stream->__wptr > stream->__bufend) || ((c == '\n') && (stream->__status & _ANVIL_STDIO_LINEBUF)))
    {
        // Flush it
        if (_Anvil_fflush(stream) == -1)
        {
            stream->__status |= _ANVIL_STDIO_ERR;
            return EOF;
        }
        // _Anvil_flush leaves the stream in an initial state so we need to
        // restore writing mode with an empty buffer
        stream->__status |= _ANVIL_STDIO_WRITING;
        stream->__wptr = stream->__buf;
        stream->__bufend = stream->__buf + stream->__buf_size - 1;
    }

    return c;
}
