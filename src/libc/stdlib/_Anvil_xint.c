
#include "_Anvil_xint.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MIN(a, b) ((a)<(b)?(a):(b))
#define MAX(a, b) ((a)>(b)?(a):(b))
#define B 0x100000000ULL

static void trim_zeroes(_Anvil_xint *x);
static int get_highest_word(_Anvil_xint *x);
static int get_highest_bit(uint32_t word);

void _Anvil_xint_init(_Anvil_xint *x, int size)
{
    x->capacity = size;
    x->size = 0;
    if ((x->data = malloc(x->capacity * sizeof(uint32_t))) == NULL)
    {
        printf("OOM\n");
        while (1);
    }
}

void _Anvil_xint_delete(_Anvil_xint *x)
{
    free(x->data);
}

void _Anvil_xint_resize(_Anvil_xint *x, int new_size)
{
    if (new_size <= x->size)
    {
        x->size = new_size;
        return;
    }
    if (new_size > x->capacity)
    {
        printf("OVER CAPACITY!!!\n");
        //x->data = realloc(x->data, new_size);
        x->capacity = new_size;
        while (1);
    }
    // Clear the new words
    for (int j=x->size; j<new_size; ++j)
    {
        x->data[j] = 0;
    }
    x->size = new_size;
}

int _Anvil_xint_is_zero(_Anvil_xint *x)
{
    for (int j=0; j<x->size; ++j)
    {
        if (x->data[j])
        {
            return 0;
        }
    }
    return 1;
}

int _Anvil_xint_print(const char *label, _Anvil_xint *x)
{
    printf("%s: ", label);
    for (int j=x->size-1; j>=0; --j)
    {
        printf("%08x ", x->data[j]);
    }
    printf("\n");
    return 1;
}

void _Anvil_xint_assign_64(_Anvil_xint *x, uint64_t val)
{
    _Anvil_xint_resize(x, val & 0xffffffff00000000U ? 2 : 1);
    x->data[0] = val & 0xffffffff;
    x->data[1] = val >> 32;
}

void _Anvil_xint_assign(_Anvil_xint *x, _Anvil_xint *y)
{
    // Check that x is big enough
    _Anvil_xint_resize(x, y->size);
    memcpy(x->data, y->data, y->size * sizeof(uint32_t));
}

uint32_t _Anvil_xint_mul_int(_Anvil_xint *x, unsigned n)
{
    uint32_t k = 0;
    for (int j=0; j<x->size; ++j)
    {
        uint64_t prod = (uint64_t)x->data[j] * n + k;
        x->data[j] = prod & 0xffffffff;
        k = prod >> 32;
    }
    if (k && (x->size < x->capacity))
    {
        x->data[x->size] = k;
        ++x->size;
        k = 0;
    }
    if (k)
    {
        printf("MUL OVERFLOW\n");
        while (1);
    }
    trim_zeroes(x);
    return k;
}

uint32_t _Anvil_xint_add(_Anvil_xint *x, _Anvil_xint *y)
{
    if (x->size < y->size)
    {
        //printf("ADD DIFF %d %d\n", x->size, y->size);
        //_Anvil_xint_print(x);
        //_Anvil_xint_print(y);
        _Anvil_xint_resize(x, y->size);
    }
    uint32_t k = 0;
    for (int j=0; j<y->size; ++j)
    {
        uint64_t sum = (uint64_t)x->data[j] + y->data[j] + k;
        x->data[j] = sum & 0xffffffff;
        k = sum >> 32;
    }
    for (int j=y->size; j<x->size; ++j)
    {
        uint64_t sum = (uint64_t)x->data[j] + k;
        x->data[j] = sum & 0xffffffff;
        k = sum >> 32;
    }
    if (k)
    {
        // We need to extend x
        _Anvil_xint_resize(x, x->size + 1);
        x->data[x->size - 1] = k;
        k = 0;
    }
    trim_zeroes(x);
    return k;
}

uint32_t _Anvil_xint_add_int(_Anvil_xint *x, unsigned n)
{
    uint32_t k = n;
    for (int j=0; (j<x->size) && k; ++j)
    {
        uint64_t sum = (uint64_t)x->data[j] + k;
        x->data[j] = sum & 0xffffffff;
        k = sum >> 32;
    }
    if (k && (x->size < x->capacity))
    {
        x->data[x->size] = k;
        ++x->size;
        k = 0;
    }
    trim_zeroes(x);
    return k;
}

int _Anvil_xint_cmp(_Anvil_xint *x, _Anvil_xint *y)
{
    int xsize = x->size;
    int ysize = y->size;
    while ((xsize > ysize) && (x->data[xsize - 1] == 0))
    {
        --xsize;
    }
    while ((ysize > xsize) && (y->data[ysize - 1] == 0))
    {
        --ysize;
    }
    if (xsize != ysize)
    {
        return xsize < ysize ? -1 : 1;
    }
    for (int j=xsize-1; j>=0; --j)
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
    if (x->size != y->size)
    {
        //printf("DIFF SIZE\n");
        
    }
    int32_t k = 0;
    for (int j=0; j<y->size; ++j)
    {
        int64_t diff = (uint64_t)x->data[j] - y->data[j] + k;
        res->data[j] = diff & 0xffffffff;
        k = diff >> 32;
    }
    for (int j=y->size; j<x->size; ++j)
    {
        int64_t diff = (uint64_t)x->data[j] + k;
        res->data[j] = diff & 0xffffffff;
        k = diff >> 32;
    }
    res->size = x->size;
    
    trim_zeroes(res);

    return k;
}

uint32_t _Anvil_xint_mul(_Anvil_xint *w, _Anvil_xint *u, _Anvil_xint *v)
{
    // Based on Knuth's algorithm M. Knuth numbers the elements from
    // the big end so this looks slightly different, but it's the same
    _Anvil_xint_resize(w, u->size + v->size);
    for (int j=0; j<w->size; ++j)
    {
        w->data[j] = 0;
    }
    for (int j=0; j<v->size; ++j)
    {
        uint32_t k = 0;
        if (v->data[j] == 0)
        {
            w->data[u->size + j] = 0;
            continue;
        }
        for (int i=0; i<u->size; ++i)
        {
            uint64_t t = (uint64_t)u->data[i] * v->data[j] + w->data[i + j] + k;
            w->data[i + j] = t & 0xffffffff;
            k = t >> 32;
        }
        w->data[u->size + j] = k;
    }
    int s;
    for (s=u->size + v->size-1; s>=0; --s)
    {
        if (w->data[s])
        {
            break;
        }
    }
    _Anvil_xint_resize(w, s+1);
    return 0;
}

const uint32_t small_pow_5[] =
{
    // 1   2   3    4     5     6      7
    1, 5, 25, 125, 625, 3125, 15625, 78125,
    // 8       9        10       11        12          13
    390625, 1953125, 9765625, 48828125, 244140625, 1220703125,
};

const uint32_t five_e8[] = { 0x0005f5e1 };

const uint32_t five_e16[] =
{
    0x86f26fc1, 0x00000023
};

const uint32_t five_e32[] =
{
    0x85acef81, 0x2d6d415b, 0x000004ee
};

const uint32_t five_e64[] =
{
    0xbf6a1f01, 0x6e38ed64, 0xdaa797ed, 0xe93ff9f4, 0x00184f03
};

const uint32_t five_e128[] =
{
    0x2e953e01, 0x03df9909, 0x0f1538fd, 0x2374e42f, 0xd3cff5ec, 0xc404dc08, 0xbccdb0da, 0xa6337f19,
    0xe91f2603, 0x0000024e
};

const uint32_t five_e256[] =
{
    0x982e7c01, 0xbed3875b, 0xd8d99f72, 0x12152f87, 0x6bde50c6, 0xcf4a6e70, 0xd595d80f, 0x26b2716e,
    0xadc666b0, 0x1d153624, 0x3c42d35a, 0x63ff540e, 0xcc5573c0, 0x65f9ef17, 0x55bc28f2, 0x80dcc7f7,
    0xf46eeddc, 0x5fdcefce, 0x000553f7
};

const uint32_t five_e512[] =
{
    0xfc6cf801, 0x77f27267, 0x8f9546dc, 0x5d96976f, 0xb83a8a97, 0xc31e1ad9, 0x46c40513, 0x94e65747,
    0xc88976c1, 0x4475b579, 0x28f8733b, 0xaa1da1bf, 0x703ed321, 0x1e25cfea, 0xb21a2f22, 0xbc51fb2e,
    0x96e14f5d, 0xbfa3edac, 0x329c57ae, 0xe7fc7153, 0xc3fc0695, 0x85a91924, 0xf95f635e, 0xb2908ee0,
    0x93abade4, 0x1366732a, 0x9449775c, 0x69be5b0e, 0x7343afac, 0xb099bc81, 0x45a71d46, 0xa2699748,
    0x8cb07303, 0x8a0b1f13, 0x8cab8a97, 0xc1d238d9, 0x633415d4, 0x0000001c
};

const uint32_t five_e1024[] =
{
    0x2919f001, 0xf55b2b72, 0x6e7c215b, 0x1ec29f86, 0x991c4e87, 0x15c51a88, 0x140ac535, 0x4c7d1e1a,
    0xcc2cd819, 0x0ed1440e, 0x896634ee, 0x7de16cfb, 0x1e43f61f, 0x9fce837d, 0x231d2b9c, 0x233e55c7,
    0x65dc60d7, 0xf451218b, 0x1c5cd134, 0xc9635986, 0x922bbb9f, 0xa7e89431, 0x9f9f2a07, 0x62be695a,
    0x8e1042c4, 0x045b7a74, 0x1abe1de3, 0x8ad822a5, 0xba34c411, 0xd814b505, 0xbf3fdeb3, 0x8fc51a16,
    0xb1b896bc, 0xf56deeec, 0x31fb6bfd, 0xb6f4654b, 0x101a3616, 0x6b7595fb, 0xdc1a47fe, 0x80d98089,
    0x80bda5a5, 0x9a202882, 0x31eb0f66, 0xfc8f1f90, 0x976a3310, 0xe26a7b7e, 0xdf68368a, 0x3ce3a0b8,
    0x8e4262ce, 0x75a351a2, 0x6cb0b6c9, 0x44597583, 0x31b5653f, 0xc356e38a, 0x35faaba6, 0x0190fba0,
    0x9fc4ed52, 0x88bc491b, 0x1640114a, 0x005b8041, 0xf4f3235e, 0x1e8d4649, 0x36a8de06, 0x73c55349,
    0xa7e6bd2a, 0xc1a6970c, 0x47187094, 0xd2db49ef, 0x926c3f5b, 0xae6209d4, 0x2d433949, 0x34f4a3c6,
    0xd4305d94, 0xd9d61a05, 0x00000325
};

const _Anvil_xint big_pow_5[] =
{
    // 8
    { 1, 1, five_e8 },
    // 16
    { 2, 2, five_e16 },
    // 32
    { 3, 3, five_e32 },
    // 64
    { 5, 5, five_e64 },
    // 128
    { 10, 10, five_e128 },
    // 256
    { 19, 19, five_e256 },
    // 512
    { 38, 38, five_e512 },
    // 1024
    { 75, 75, five_e1024 },
};

int smallest_div = 0;
int biggest_mul = 0;

uint32_t _Anvil_xint_mul_5exp(_Anvil_xint *x, int e)
{
    if (e < 0)
    {
        //printf("Doing DIV\n");
        return _Anvil_xint_div_5exp(x, -e);
    }
    if (e > biggest_mul)
    {
        printf("Doing mul %d\n", e);
        biggest_mul = e;
    }

    _Anvil_xint tmp;
    _Anvil_xint_init(&tmp, x->capacity);
    _Anvil_xint_mul_int(x, small_pow_5[e & 0x7]);
    _Anvil_xint_assign(&tmp, x);
    e >>= 3;
    int ndx = 0;
    while (e)
    {
        if (e & 1)
        {
            if (ndx >= sizeof(big_pow_5) / sizeof(big_pow_5[0]))
            {
                printf("TOO BIG %d\n", ndx);
                while (1);
            }
            _Anvil_xint_mul(&tmp, x, &big_pow_5[ndx]);
            _Anvil_xint_assign(x, &tmp);
        }
        ++ndx;
        e >>= 1;
    }
    _Anvil_xint_delete(&tmp);
    return 0;
}

uint32_t _Anvil_xint_div_5exp(_Anvil_xint *x, int e)
{
    if (e > smallest_div)
    {
        printf("Doing div %d\n", e);
        smallest_div = e;
    }
    const int highest_small_e5 = sizeof(small_pow_5) / sizeof(small_pow_5[0]) - 2;
    while (e > highest_small_e5)
    {
        _Anvil_xint_div_int(x, x, small_pow_5[highest_small_e5]);
        e -= highest_small_e5;
    }
    _Anvil_xint_div_int(x, x, small_pow_5[e]);
    return 0;
}

int qq1, qq2;

uint32_t _Anvil_xint_div_small(_Anvil_xint *u, _Anvil_xint *v)
{
    // Find the highest bit in the highest word in v that contains data
    int highest_word = get_highest_word(v);

    // Move both u and v to the left so that the top bit of V is set
    uint32_t denom;
    uint64_t numer;

    if (highest_word > 0)
    {
        denom = v->data[highest_word] + 1;

        if (highest_word == u->size - 2)
        {
            // U is longer than V
            numer = u->data[u->size - 1];
            numer <<= 32;
            numer |= u->data[u->size - 2];
        }
        else if (highest_word == u->size - 1)
        {
            // U and V are equal length
            numer = u->data[u->size - 1];
        }
        else
        {
            // U is shorter than V
            numer = 0;
        }
    }
    else
    {
        denom = v->data[0] + 1;
        numer = u->data[0];
    }

    uint64_t quot = numer / denom;

    if (u->size < v->size)
    {
        _Anvil_xint_resize(u, v->size);
    }
    else if (u->size > v->size)
    {
        _Anvil_xint_resize(v, u->size);
    }

    int64_t k = 0;
    for (int i=0; i<u->size; ++i)
    {
        int64_t prod_diff = u->data[i] - quot * v->data[i] + k;
        u->data[i] = prod_diff & 0xffffffff;
        k = prod_diff >> 32;
    }

    ++qq1;
    while (_Anvil_xint_cmp(u, v) >= 0)
    {
        ++qq2;
        _Anvil_xint_sub(u, u, v);
        ++quot;
    }
    trim_zeroes(u);
    trim_zeroes(v);
    return quot;
}

int _Anvil_xint_highest_bit(_Anvil_xint *x)
{
    // Find the highest bit in the highest word in v that contains data
    int highest_word = get_highest_word(x);
    int highest_bit = get_highest_bit(x->data[highest_word]);
    return highest_word * 32 + highest_bit;
}

uint32_t _Anvil_xint_div(_Anvil_xint *q, _Anvil_xint *r, _Anvil_xint *u, _Anvil_xint *v)
{
    // As per Knuth's later revision with little endian indices
    // u[0] to u[m+n-1]
    // v[0] to v[n-1]
    // q[0] to q[m]
    // r[0] to r[n-1]

    if (v->size == 1)
    {
        // Use the algorithm from exercise 16
        uint32_t a = _Anvil_xint_div_int(q, u, v->data[0]);
        _Anvil_xint_assign_64(r, a);
        trim_zeroes(q);
        trim_zeroes(r);
        return 0;
    }

    // D1. [Normalise.]
    // Use Knuth's suggestion of a power of 2 for d. For v1 to be > b/2
    // we need v1 to have its top bit set.

    // Find the highest bit in the highest word in v that contains data
    int highest_word = get_highest_word(v);
    int highest_bit = get_highest_bit(v->data[highest_word]);

    // Move both u and v to the left so that the top bit of V is set
    // Note that we use r for normalised u from now on
    _Anvil_xint_lshift(r, u, 31 - highest_bit);
    _Anvil_xint_lshift(v, v, 31 - highest_bit);

    int n = v->size;
    int m = r->size - n + 1;

    _Anvil_xint_assign_64(q, 0);
    _Anvil_xint_resize(q, m + n);
    _Anvil_xint_resize(r, m + n + 1);

    // D2. [Initialise j.]
    for (int j=m; j>=0; --j)
    {
        // D3. [Calculate q^]
        uint64_t qhat = (r->data[j+n] * B + r->data[j+n-1]) / v->data[n-1];
        uint64_t rhat = (r->data[j+n] * B + r->data[j+n-1]) % v->data[n-1];
        while (qhat >= B || (qhat * v->data[n-2] > B * rhat + r->data[j+n-2]))
        {
            --qhat;
            rhat += v->data[n-1];
            if (rhat >= B)
            {
                break;
            }
        }

        // D4. [Multiply and subtract.]
        // Replace u(j+n to j) by u(j+n to j) - qhat * v(n-1 to 0)
        // Since r is u we have r = r - qhat * v
        int64_t k = 0;
        for (int i=0; i<n; ++i)
        {
            int64_t prod_diff = r->data[j+i] - qhat * v->data[i] + k;
            r->data[i+j] = prod_diff & 0xffffffff;
            k = prod_diff >> 32;
        }
        int64_t prod_diff = r->data[j+n] + k;
        r->data[j+n] = prod_diff & 0xffffffff;

        // Note that we know qhat < B
        q->data[j] = (uint32_t)qhat;

        // D5. Test remainder
        if (prod_diff < 0)
        {
            // D6. [Add back.]
            // Decrease Qj by one
            --q->data[j];
            // Add V(n-1 to 0) to U(n+j to j)
            int64_t k = 0;
            for (int i=0; i<n; i++)
            {
                uint64_t sum = (uint64_t)r->data[j+i] + v->data[i] + k;
                r->data[j+i] = sum & 0xffffffff;
                k = sum >> 32;
            }
            r->data[j+n] = (r->data[j+n] + k) & 0xffffffff;
        }
        // D7. Loop on j
    }

    // D8. Un-normalise
    _Anvil_xint_rshift(v, v, 31 - highest_bit);
    _Anvil_xint_rshift(r, r, 31 - highest_bit);

    trim_zeroes(q);
    trim_zeroes(r);
    trim_zeroes(v);

    return 0;
}

uint32_t _Anvil_xint_div_int(_Anvil_xint *quot, _Anvil_xint *x, uint32_t v)
{
    // This is from Knuth's recommended exercise 16 with the indices reversed
    uint32_t r = 0;
    _Anvil_xint_resize(quot, x->size);
    for (int j=x->size-1; j>=0; --j)
    {
        uint64_t tmp = ((uint64_t)r << 32) + x->data[j];
        quot->data[j] = (uint32_t)(tmp / v);
        r = tmp % v;
    }
    trim_zeroes(quot);
    return r;
}

uint32_t _Anvil_xint_lshift(_Anvil_xint *y, _Anvil_xint *x, int numbits)
{
    if (numbits == 0)
    {
        return 0;
    }
    if (numbits < 0)
    {
        return _Anvil_xint_rshift(y, x, -numbits);
    }
    // Calculate the shift
    int shift_words = numbits / 32;
    int shift_bits = numbits % 32;

    // Find the highest word in x that contains data
    int highest_word = get_highest_word(x);
    int highest_bit = get_highest_bit(x->data[highest_word]);
    
    if (shift_bits == 0)
    {
        _Anvil_xint_resize(y, x->size + shift_words);
        for (int j=y->size-1; j>=shift_words; --j)
        {
            y->data[j] = x->data[j - shift_words];
        }
        for (int j=shift_words-1; j>=0; --j)
        {
            y->data[j] = 0;
        }
    }
    else
    {
        _Anvil_xint_resize(y, x->size + shift_words);
        for (int j=y->size-1; j>=shift_words; --j)
        {
            y->data[j] = x->data[j - shift_words];
        }
        for (int j=shift_words-1; j>=0; --j)
        {
            y->data[j] = 0;
        }
        if (shift_bits + highest_bit > 31)
        {
            _Anvil_xint_resize(y, y->size + 1);
        }
        for (int j=y->size-1; j>0; --j)
        {
            y->data[j] = (y->data[j] << shift_bits) | (y->data[j - 1] >> (32 - shift_bits));
        }
        y->data[0] = y->data[0] << shift_bits;
    }

    trim_zeroes(y);

    return 0;
}

uint32_t _Anvil_xint_rshift(_Anvil_xint *y, _Anvil_xint *x, int numbits)
{
    // Calculate the shift
    int shift_words = numbits / 32;
    int shift_bits = numbits % 32;

    // Find the highest word in x that contains data
    int highest_word = get_highest_word(x);
    int highest_bit = get_highest_bit(x->data[highest_word]);
    
    // If all of x will be shifted out ...
    if (x->size <= shift_words)
    {
        // set y to 0
        _Anvil_xint_assign_64(y, 0);
        return 0;
    }
    
    // We need y->size to be at least x->size - shift_words in size
    if (y->size < x->size - shift_words)
    {
        _Anvil_xint_resize(y, x->size - shift_words);
    }
    if (shift_words || (y != x))
    {
        // Work from right to left
        for (int j=0; j<x->size - shift_words; ++j)
        {
            y->data[j] = x->data[j + shift_words];
        }
    }
    if (shift_bits)
    {
        uint64_t tmp = 0;
        for (int j=0; j<x->size - shift_words - 1; ++j)
        {
            tmp = y->data[j+1];
            tmp <<= (32 - shift_bits);
            tmp |= y->data[j] >> shift_bits;
            y->data[j] = tmp & 0xffffffff;
        }
        y->data[x->size - shift_words - 1] >>= shift_bits;
    }
    
    trim_zeroes(y);

    return 0;
}

static void trim_zeroes(_Anvil_xint *x)
{
    for (int j=x->size-1; j>=0; --j)
    {
        if (x->data[j] != 0)
        {
            x->size = j + 1;
            break;
        }
        if (j == 0)
        {
            x->size = 0;
            break;
        }
    }
}

static int get_highest_word(_Anvil_xint *x)
{
    for (int highest_word=x->size-1; highest_word>=0; --highest_word)
    {
        if (x->data[highest_word])
        {
            return highest_word;
        }
    }
    return -1;
}

static int get_highest_bit(uint32_t word)
{
    if (word)
    {
        return 31 - __builtin_clz(word);
    }
    return -1;
}
