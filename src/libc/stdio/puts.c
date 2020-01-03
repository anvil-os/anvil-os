
#include "_Anvil_stdio.h"

int puts(const char *s)
{
    int ret = 0;
    _Anvil_flockfile(stdout);
    if (fputs(s, stdout) == EOF || fputc('\n', stdout) == EOF)
    {
        ret = EOF;
    }
    _Anvil_funlockfile(stdout);
    return ret;
}
