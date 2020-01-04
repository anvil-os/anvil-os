
#include "_Anvil_stdio.h"

int _Anvil_fgetc(FILE *stream)
{
    long bytes_read;

    // When reading the buffer is slightly different from when writing in that
    // the __bufend pointer may not point to just past the end of the buffer
    // when a 'short' read is made. i.e. when at EOF
    // The buffer below can hold 8 entries but only contains 4 because that is
    // all that were available
    // Note that we don't return EOF yet since another thread may be writing
    // and another read may succeed. EOF should only be returned when a read()
    // returns 0 indicating EOF.
    //
    // Unbuffered streams have a single byte buffer. This ensures this function
    // is entered and a read() occurs on every byte read
    //
    //  |-----|-----|-----|-----|-----|-----|
    //  |  T  |  h  |  e  |     |     |     |
    //  |-----|-----|-----|-----|-----|-----|
    //     ^           ^     ^                 ^
    //     |           |     |                 |
    //     |           |     |--- __bufend     |-- __bufend is here if
    //     |           |                           the buffer is full
    //     |           |
    //     |           |
    //     |           |
    //     |           |--------- __rptr - the next char to be returned
    //     |
    //     |--------------------- __buf
    //
    //

    // If there is data in the buffer we must have been through here before
    // and filled it so we can return without any further checks
    if (stream->__rptr < stream->__bufend)
    {
        return *stream->__rptr++;
    }

    // Check for EOF first
    if (stream->__status & _ANVIL_STDIO_EOF)
    {
        return EOF;
    }

    // Check that we are allowed to read -
    // a) must be readable (or read/writable)
    // b) must not be writing
    // c) must not be wide
    // d) must not be error
    if ( !(stream->__status & (_ANVIL_STDIO_READABLE))  ||
          (stream->__status & (_ANVIL_STDIO_WRITING | _ANVIL_STDIO_WIDE | _ANVIL_STDIO_ERR)))
    {
        stream->__status |= _ANVIL_STDIO_ERR;
        return EOF;
    }

    // Commit to being a byte stream in read mode
    stream->__status |= (_ANVIL_STDIO_BYTE | _ANVIL_STDIO_READING);

    // If there is not a buffer yet, create one. Also initialises the pointers in the
    // FILE structure
    if (stream->__buf == NULL)
    {
        // This function guarantees a buffer of some type
        _Anvil_setbuf(stream);
    }

    // Todo: set all other pointers here?
    stream->__rptr = stream->__buf;

    // If we get here we have a completely empty buffer so fill it
    if ((bytes_read = _Anvil_read(stream->__fd, stream->__buf, stream->__buf_size)) < 0)
    {
        stream->__status |= _ANVIL_STDIO_ERR;
        return EOF;
    }

    if (bytes_read == 0)
    {
        // We're at EOF
        stream->__status |= _ANVIL_STDIO_EOF;
        return EOF;
    }

    // Set __bufend to point just beyond
    stream->__bufend = stream->__buf + bytes_read;

    // Return the first char from the buffer
    return *stream->__rptr++;
}
