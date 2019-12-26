
#include "_Anvil_stdio.h"

int fputs(const char *restrict s, FILE *restrict stream)
{
    int ret = 0;
    _Anvil_flockfile(stream);
    while (*s)
    {
        if (_Anvil_fputc_unlocked(*s, stream) == EOF)
        {
            ret = EOF;
            break;
        }
        ++s;
    }
    _Anvil_funlockfile(stream);
    return ret;
}
