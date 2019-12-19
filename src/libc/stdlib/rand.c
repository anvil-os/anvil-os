
#include <stdlib.h>

static unsigned long int _RandSeed = 1;

int rand(void)
{
    // Use a simple LCG, Xn+1 = (aXn + c ) % m
    //
    // Traditionally in Ansi
    //
    // m = 2 ^ 32
    // a = 1103515245
    // c = 12345
    //
    // we return bits 30..16 to limit the output to RAND_MAX
    //
    _RandSeed = 1103515245 * _RandSeed + 12345;
    return (_RandSeed >> 16) % 32768;
}

void srand(unsigned seed)
{
    _RandSeed = seed;
}
