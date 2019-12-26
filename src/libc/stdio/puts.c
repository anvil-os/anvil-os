
#include "_Anvil_stdio.h"

int puts(const char *s)
{
    int ret = 0;
    _Anvil_flockfile(stdout);
    if (fputs(s, stdout) != 0 || fputc('\n', stdout) != 0)
    {
        ret = EOF;
    }
    _Anvil_funlockfile(stdout);
    return ret;
}
