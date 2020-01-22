
#ifndef _ANVIL_XINT_H
#define _ANVIL_XINT_H

#include <stdint.h>

#define NPLACES 64

struct _Anvil_xint_s
{
    // This stores the data in little-endian format. This will simplify
    // growing and shrinking the array - NYI
    uint32_t data[NPLACES];
};

typedef struct _Anvil_xint_s _Anvil_xint;

void _Anvil_xint_init(_Anvil_xint *x);
void _Anvil_xint_assign_64(_Anvil_xint *x, uint64_t val);
void _Anvil_xint_assign(_Anvil_xint *x, _Anvil_xint *y);
uint32_t _Anvil_xint_mul_int(_Anvil_xint *x, unsigned n);
uint32_t _Anvil_xint_add_int(_Anvil_xint *x, unsigned n);
int _Anvil_xint_cmp(_Anvil_xint *x, _Anvil_xint *y);
uint32_t _Anvil_xint_sub(_Anvil_xint *res, _Anvil_xint *x, _Anvil_xint *y);
uint32_t _Anvil_xint_mul(_Anvil_xint *res, _Anvil_xint *x, _Anvil_xint *y);
uint32_t _Anvil_xint_mul_10exp(_Anvil_xint *x, int e);
uint32_t _Anvil_xint_mul_2exp(_Anvil_xint *x, int e);

#endif /* _ANVIL_XINT_H */
