
#include "_Anvil_stdio.h"

int fputc(int c, FILE *stream)
{
    int ret;
    _Anvil_flockfile(stream);
    ret = _Putc_unlocked(c, stream);
    _Anvil_funlockfile(stream);
    return ret;
}
