
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

const uint32_t small_pow_5[] =
{
    // 1   2   3    4     5     6      7
    1, 5, 25, 125, 625, 3125, 15625, 78125,
};

// XXX: This table is still too big
const _Anvil_xint big_pow_5[] =
{
    // 8
    {{ 0x0005f5e1 }},
    // 16
    {{ 0x86f26fc1, 0x00000023 }},
    // 32
    {{ 0x85acef81, 0x2d6d415b, 0x000004ee }},
    // 64
    {{ 0xbf6a1f01, 0x6e38ed64, 0xdaa797ed, 0xe93ff9f4, 0x00184f03 }},
    // 128
    {{ 0x2e953e01, 0x03df9909, 0x0f1538fd, 0x2374e42f, 0xd3cff5ec, 0xc404dc08, 0xbccdb0da, 0xa6337f19,
       0xe91f2603, 0x0000024e }},
    // 256
    {{ 0x982e7c01, 0xbed3875b, 0xd8d99f72, 0x12152f87, 0x6bde50c6, 0xcf4a6e70, 0xd595d80f, 0x26b2716e,
       0xadc666b0, 0x1d153624, 0x3c42d35a, 0x63ff540e, 0xcc5573c0, 0x65f9ef17, 0x55bc28f2, 0x80dcc7f7,
       0xf46eeddc, 0x5fdcefce, 0x000553f7 }},
    // 512
    {{ 0xfc6cf801, 0x77f27267, 0x8f9546dc, 0x5d96976f, 0xb83a8a97, 0xc31e1ad9, 0x46c40513, 0x94e65747,
       0xc88976c1, 0x4475b579, 0x28f8733b, 0xaa1da1bf, 0x703ed321, 0x1e25cfea, 0xb21a2f22, 0xbc51fb2e,
       0x96e14f5d, 0xbfa3edac, 0x329c57ae, 0xe7fc7153, 0xc3fc0695, 0x85a91924, 0xf95f635e, 0xb2908ee0,
       0x93abade4, 0x1366732a, 0x9449775c, 0x69be5b0e, 0x7343afac, 0xb099bc81, 0x45a71d46, 0xa2699748,
       0x8cb07303, 0x8a0b1f13, 0x8cab8a97, 0xc1d238d9, 0x633415d4, 0x0000001c
    }}
};

uint32_t _Anvil_xint_mul_5exp(_Anvil_xint *x, int e)
{
    _Anvil_xint tmp;
    _Anvil_xint_mul_int(x, small_pow_5[e & 0x7]);
    _Anvil_xint_assign(&tmp, x);
    e >>= 3;
    int ndx = 0;
    while (e)
    {
        if (e & 1)
        {
            _Anvil_xint_mul(&tmp, x, &big_pow_5[ndx]);
            _Anvil_xint_assign(x, &tmp);
        }
        ++ndx;
        e >>= 1;
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
