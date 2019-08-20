
#include <stdio.h>
#include "debug.h"

int puts(const char *s)
{
    return printf("%s\n", s);
}
