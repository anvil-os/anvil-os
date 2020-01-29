
#include "_Anvil_xint.h"

#include <stdlib.h>
#include <string.h>

void _Anvil_xint_init(_Anvil_xint *x)
{
    memset(x->data, 0, NPLACES * sizeof(uint32_t));
}

int _Anvil_xint_is_zero(_Anvil_xint *x)
{
    for (int j=0; j<NPLACES; ++j)
    {
        if (x->data[j])
        {
            return 0;
        }
    }
    return 1;
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
    for (int j=0; j<NPLACES; ++j)
    {
        uint64_t prod = (uint64_t)x->data[j] * n + k;
        x->data[j] = prod & 0xffffffff;
        k = prod >> 32;
    }
    return k;
}

uint32_t _Anvil_xint_add(_Anvil_xint *x, _Anvil_xint *y)
{
    uint32_t k = 0;
    for (int j=0; j<NPLACES; ++j)
    {
        uint64_t sum = (uint64_t)x->data[j] + y->data[j] + k;
        x->data[j] = sum & 0xffffffff;
        k = sum >> 32;
    }
    return k;
}

uint32_t _Anvil_xint_add_int(_Anvil_xint *x, unsigned n)
{
    uint32_t k = n;
    for (int j=0; (j<NPLACES) && k; ++j)
    {
        uint64_t sum = (uint64_t)x->data[j] + k;
        x->data[j] = sum & 0xffffffff;
        k = sum >> 32;
    }
    return k;
}

int _Anvil_xint_cmp(_Anvil_xint *x, _Anvil_xint *y)
{
    for (int j=NPLACES-1; j>=0; --j)
    {
        if (x->data[j] != y->data[j])
        {
            return x->data[j] < y->data[j] ? -1 : 1;
        }
    }
    return 0;
}

uint32_t _Anvil_xint_sub(_Anvil_xint *res, _Anvil_xint *x, _Anvil_xint *y)
{
    // Based on Knuth's algorithm S
    uint32_t k = 0;
    for (int j=0; j<NPLACES; ++j)
    {
        uint64_t diff = (uint64_t)x->data[j] - y->data[j] + (k ? (uint64_t)-1 : 0);
        res->data[j] = diff & 0xffffffff;
        k = diff >> 32;
    }
    return k;
}

uint32_t _Anvil_xint_mul(_Anvil_xint *w, _Anvil_xint *u, _Anvil_xint *v)
{
    // Based on Knuth's algorithm M. Knuth numbers the elements from
    // the big end so this looks slightly different, but it's the same
    // Todo: XXX: get rid of this!!!
    uint32_t temp[NPLACES * 2];
    memset(temp, 0, 2 * NPLACES * sizeof(uint32_t));

    for (int j=0; j<NPLACES; ++j)
    {
        uint32_t k = 0;
        if (v->data[j] == 0)
        {
            temp[NPLACES + j] = 0;
            continue;
        }
        for (int i=0; i<NPLACES; ++i)
        {
            uint64_t t = (uint64_t)u->data[i] * v->data[j] + temp[i + j] + k;
            temp[i + j] = t & 0xffffffff;
            k = t >> 32;
        }
        temp[NPLACES + j] = k;
    }
    memcpy(w->data, temp, NPLACES * sizeof(uint32_t));
    return 0;
}

uint32_t _Anvil_xint_mul_10exp(_Anvil_xint *x, int e)
{
    // This is far too slow - FIX
    for (int j=0; j<e; ++j)
    {
        _Anvil_xint_mul_int(x, 10);
    }
    return 0;
}

uint32_t _Anvil_xint_mul_2exp(_Anvil_xint *x, int e)
{
    // This is far too slow - make it a left shift
    for (int j=0; j<e; ++j)
    {
        _Anvil_xint_mul_int(x, 2);
    }
    return 0;
}

uint32_t _Anvil_xint_div(_Anvil_xint *rem, _Anvil_xint *u, _Anvil_xint *v)
{
    // Since we know that the quotient will be 0 - 9 let's use simple subtraction
    // and count how many
    uint32_t quot = 0;
    _Anvil_xint_assign(rem, u);
    while (_Anvil_xint_cmp(rem, v) >= 0)
    {
        _Anvil_xint_sub(rem, rem, v);
        ++quot;
    }
    return quot;
}

uint32_t _Anvil_xint_div_int(_Anvil_xint *quot, _Anvil_xint *x, uint32_t v)
{
    // This is from Kntuth's recommended exercise 16 with the indices reversed
    uint32_t r = 0;
    for (int j=NPLACES-1; j>=0; --j)
    {
        uint64_t tmp = ((uint64_t)r << 32) + x->data[j];
        quot->data[j] = (uint32_t)(tmp / v);
        r = tmp % v;
    }
    return r;
}

uint32_t _Anvil_xint_lshift(_Anvil_xint *y, _Anvil_xint *x, int nbits)
{
    if (nbits >= 32)
    {
        int nwords = nbits / 32;
        for (int j=NPLACES-1; j>=nwords; --j)
        {
            y->data[j] = x->data[j - nwords];
        }
        for (int j=nwords-1; j>=0; --j)
        {
            y->data[j] = 0;
        }
        nbits -= 32 * nwords;
    }
    if (nbits)
    {
        uint64_t tmp = 0;
        for (int j=NPLACES-1; j>0; --j)
        {
            tmp = x->data[j - 1];
            tmp <<= nbits;
            y->data[j] = x->data[j] << nbits;
            y->data[j] |= tmp >> 32;
        }
        y->data[0] = x->data[0] << nbits;
    }
    return 0;
}
