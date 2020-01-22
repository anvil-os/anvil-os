
#include "_Anvil_xint.h"

#include <stdlib.h>
#include <string.h>

void _Anvil_xint_init(_Anvil_xint *x)
{
    memset(x->data, 0, NPLACES * sizeof(uint32_t));
}

void _Anvil_xint_assign_64(_Anvil_xint *x, uint64_t val)
{
    memset(x->data, 0, NPLACES * sizeof(uint32_t));
    x->data[0] = val & 0xffffffff;
    x->data[1] = val >> 32;
}

void _Anvil_xint_assign(_Anvil_xint *x, _Anvil_xint *y)
{
    memcpy(x->data, y->data, NPLACES * sizeof(uint32_t));
}

uint32_t _Anvil_xint_mul_int(_Anvil_xint *x, unsigned n)
{
    uint32_t k = 0;
    int i;
    for (i = 0; i < NPLACES; ++i)
    {
        uint64_t prod = (uint64_t)x->data[i] * n + k;
        x->data[i] = prod & 0xffffffff;
        k = (prod >> 32) & 0xffffffff;
    }
    return k;
}

uint32_t _Anvil_xint_add_int(_Anvil_xint *x, unsigned n)
{
    uint32_t k = n;
    int i;
    for (i = 0; (i < NPLACES) && k; ++i)
    {
        uint64_t sum = (uint64_t)x->data[i] + k;
        x->data[i] = sum & 0xffffffff;
        k = (sum >> 32) & 0xffffffff;
    }
    return k;
}

int _Anvil_xint_cmp(_Anvil_xint *x, _Anvil_xint *y)
{
    int i;
    for (i = NPLACES - 1; i >= 0; --i)
    {
        if (x->data[i] != y->data[i])
        {
            return x->data[i] < y->data[i] ? -1 : 1;
        }
    }
    return 0;
}

uint32_t _Anvil_xint_sub(_Anvil_xint *res, _Anvil_xint *x, _Anvil_xint *y)
{
    // Based on Knuth's algorithm S
    uint32_t k = 0;
    int i;
    for (i = 0; (i < NPLACES); ++i)
    {
        uint64_t diff = (uint64_t)x->data[i] - y->data[i] + (k ? (uint64_t)-1 : 0);
        res->data[i] = diff & 0xffffffff;
        k = diff >> 32;
    }
    return k;
}

uint32_t _Anvil_xint_mul(_Anvil_xint *w, _Anvil_xint *u, _Anvil_xint *v)
{
    // Based on Knuth's algorithm M. Knuth numbers the elements from
    // the big end so this looks slightly different, but it's the same
    uint32_t temp[NPLACES * 2];
    memset(temp, 0, 2 * NPLACES * sizeof(uint32_t));

    for (int j = 0; j < NPLACES; ++j)
    {
        uint32_t k = 0;
        if (v->data[j] == 0)
        {
            temp[NPLACES + j] = 0;
            continue;
        }
        for (int i = 0; i < NPLACES; ++i)
        {
            uint64_t t = (uint64_t)u->data[i] * v->data[j] + temp[i + j] + k;
            temp[i + j] = t & 0xffffffff;
            k = (t >> 32) & 0xffffffff;
        }
        temp[NPLACES + j] = k;
    }
    memcpy(w->data, temp, NPLACES * sizeof(uint32_t));
    return 0;
}

uint32_t _Anvil_xint_mul_10exp(_Anvil_xint *x, int e)
{
    // This is far too slow - FIX
    for (int i=0; i<e; ++i)
    {
        _Anvil_xint_mul_int(x, 10);
    }
    return 0;
}

uint32_t _Anvil_xint_mul_2exp(_Anvil_xint *x, int e)
{
    // This is far too slow - make it a left shift
    for (int i=0; i<e; ++i)
    {
        _Anvil_xint_mul_int(x, 2);
    }
    return 0;
}
