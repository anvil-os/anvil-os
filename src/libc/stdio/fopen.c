
#include "_Anvil_stdio.h"
#include <stdlib.h>

FILE *fopen(const char *restrict filename, const char *restrict mode)
{
    const char *pmode = mode;
    FILE *f;
    int open_mode = 0;
    int saw_b = 0;
    int saw_plus = 0;
    int stream_mode = 0;
    int fd;

    // The first char can be r, w, or a
    switch (*pmode)
    {
        case 'r':
            open_mode = O_RDONLY;
            stream_mode = _ANVIL_STDIO_READABLE;
            break;
        case 'w':
            open_mode = O_WRONLY | O_CREAT | O_TRUNC;
            stream_mode = _ANVIL_STDIO_WRITEABLE;
            break;
        case 'a':
            open_mode = O_WRONLY | O_CREAT | O_APPEND;
            stream_mode = _ANVIL_STDIO_WRITEABLE;
            break;
        default:
            return NULL;
    }
    ++pmode;

    // b and/or +
    while (1)
    {
        switch (*pmode)
        {
            case '+':
                if (saw_plus)
                {
                    return NULL;
                }
                saw_plus = 1;
                open_mode &= ~(O_RDONLY | O_WRONLY);
                open_mode |= O_RDWR;
                stream_mode |= (_ANVIL_STDIO_WRITEABLE | _ANVIL_STDIO_READABLE);
                ++pmode;
                continue;

            case 'b':
                if (saw_b)
                {
                    return NULL;
                }
                saw_b = 1;
                // We ignore 'b'
                ++pmode;
                continue;

            default:
                break;
        }
        break;
    }

    // Optional x if first char is w
    if (*pmode == 'x' && mode[0] == 'w')
    {
        open_mode |= O_EXCL;
    }

    if ((f = (FILE *)calloc(1, sizeof(FILE))) == NULL)
    {
        return NULL;
    }

    f->__fd = _Anvil_open(filename, open_mode);
    f->__status = stream_mode | _ANVIL_STDIO_FILE_MALLOCED;

    return f;
}
