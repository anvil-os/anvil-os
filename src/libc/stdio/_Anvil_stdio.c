
#include "_Anvil_stdio.h"

FILE _Anvil_stdin  = { 0 };
FILE _Anvil_stdout = { 1 };
FILE _Anvil_stderr = { 2 };

FILE *stdin = &_Anvil_stdin;
FILE *stdout = &_Anvil_stdout;
FILE *stderr = &_Anvil_stderr;
