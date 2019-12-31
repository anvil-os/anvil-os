
#include "_Anvil_stdio.h"

FILE _Anvil_stdin  = { 0, _ANVIL_STDIO_READABLE,  0, 0, 0, 0 };
FILE _Anvil_stdout = { 1, _ANVIL_STDIO_WRITEABLE, 0, 0, 0, 0 };
FILE _Anvil_stderr = { 2, _ANVIL_STDIO_WRITEABLE, 0, 0, 0, 0 };

FILE *stdin = &_Anvil_stdin;
FILE *stdout = &_Anvil_stdout;
FILE *stderr = &_Anvil_stderr;
