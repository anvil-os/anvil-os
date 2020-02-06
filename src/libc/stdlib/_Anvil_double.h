

#include <stdint.h>

#define MAX(a, b) ((a)>(b)?(a):(b))

enum
{
    e_normal,
    e_absolute,
    e_relative,
};

union double_bits
{
    double dbl;
    uint64_t uint;
};

static void split_double(double dd, int *sign, uint64_t *f, int *e)
{
    union double_bits value;
    value.dbl = dd;
    *sign = value.uint >> 63;
    *e = ((value.uint >> 52) & 0x7ff) - 1023;
    *f = value.uint & 0xfffffffffffff;
    if (*e == -1023)
    {
        ++*e;
    }
    else
    {
        *f |= 0x10000000000000;
    }
}
