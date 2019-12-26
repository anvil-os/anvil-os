
#include <stdio.h>

size_t fwrite(const void *restrict ptr, size_t size, size_t nmemb, FILE *restrict stream)
{
    const unsigned char *p = ptr;
    size_t bytes;

    if (size == 0 || nmemb == 0)
    {
        // Nothing to write so we are done
        return 0;
    }

    bytes = size * nmemb;
    if (bytes < size || bytes < nmemb)
    {
        // Overflow
        return 0;
    }

    _Anvil_flockfile(stream);
    while (bytes)
    {
        if (_Anvil_fputc_unlocked(*p, stream) == EOF)
        {
            break;
        }
        ++p;
        --bytes;
    }
    _Anvil_funlockfile(stream);

    return (p - (const unsigned char *)ptr) / size;;
}
