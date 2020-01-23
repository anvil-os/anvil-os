
#include <stdlib.h>

#include <limits.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>

#include "_Anvil_xint.h"

static const int n = 53;
static const int p = 64;

uint64_t two_to_p = 1LL << p;                // 2^64
uint64_t two_to_n = 1LL << n;                // 2^53
int64_t two_to_p_n_1 = 1LL << (p - n - 1);   // 2^10
uint64_t two_to_p_minus_n = 1LL << (p - n);  // 2^9
uint64_t two_to_n_minus_1 = 1LL << (n - 1);  // 2^52
uint64_t two_to_n_plus_1 = 1LL << (n + 1);  // 2^54
uint64_t log5_of_two = 23;  // ceil(log two_to_n / log 5)

uint64_t float_significand(double z)
{
    union
    {
        double dbl;
        uint64_t uint;
    } z_z;
    z_z.dbl = z;
    uint64_t sig = z_z.uint & 0xfffffffffffff;
    sig |= 0x10000000000000;
    return sig;
}

int float_exponent(double z)
{
    union
    {
        double dbl;
        uint64_t uint;
    } z_z;
    z_z.dbl = z;
    int e = ((z_z.uint >> 52) & 0x7ff);
    e -= 1023;
    e -= 52;
    return e;
}

double make_float(uint64_t m, int k)
{
    union
    {
        double dbl;
        uint64_t uint;
    } z_z;
    z_z.uint = m;
    z_z.uint &= ~0x0010000000000000;
    k += 1023;
    k += 52;
    z_z.uint |= ((uint64_t)k << 52);
    return z_z.dbl;
}

double prev_float(double z)
{
    uint64_t m = float_significand(z);
    int k = float_exponent(z);
    if (m == two_to_n_minus_1)
    {
        return make_float(two_to_n - 1, k - 1);
    }
    else
    {
        return make_float(m - 1, k);
    }
}

double next_float(double z)
{
    uint64_t m = float_significand(z);
    int k = float_exponent(z);
    if (m == two_to_n - 1)
    {
        return make_float(two_to_n_minus_1, k + 1);
    }
    else
    {
        return make_float(m + 1, k);
    }
}

double algoritm_r(_Anvil_xint *f, int e, double z0)
{
    double z = z0;
    _Anvil_xint x;
    _Anvil_xint y;
    _Anvil_xint D_abs;
    _Anvil_xint D2;
    _Anvil_xint M;
    _Anvil_xint_init(&x);
    _Anvil_xint_init(&y);
    _Anvil_xint_init(&D_abs);
    _Anvil_xint_init(&D2);
    _Anvil_xint_init(&M);

    int loop = 0;

    while (1)
    {
        //dump_double(z);
        uint64_t m = float_significand(z);
        int k = float_exponent(z);

        //printf("m=%lld k=%d\n", m, k);

        _Anvil_xint_assign(&x, f);

        _Anvil_xint_assign_64(&y, m);

        if (e >= 0)
        {
            if (k >= 0)
            {
                // x = f * 10^e
                _Anvil_xint_mul_10exp(&x, e);
                // y = m * 2^k
                _Anvil_xint_mul_2exp(&y, k);
            }
            else
            {
                // x = f * 10^e * 2^-k
                _Anvil_xint_mul_10exp(&x, e);
                _Anvil_xint_mul_2exp(&x, -k);
                // y = m
                ;
            }
        }
        else
        {
            if (k >= 0)
            {
                // x = f
                ;
                // y = m * 2^k * 10^-e
                _Anvil_xint_mul_2exp(&y, k);
                _Anvil_xint_mul_10exp(&y, -e);
            }
            else
            {
                // x = f * 2^-k
                _Anvil_xint_mul_2exp(&x, -k);
                // y = m * 10^-e
                _Anvil_xint_mul_10exp(&y, -e);
            }
        }

        // D = x - y
        int D_sign = _Anvil_xint_cmp(&x, &y);
        if (D_sign > 0)
        {
            _Anvil_xint_sub(&D_abs, &x, &y);
        }
        else
        {
            _Anvil_xint_sub(&D_abs, &y, &x);
        }

        //printf("D2 abs %lx %lx\n", D_abs.data[0], D_abs.data[1]);

        // D2 = 2 * m * abs(d)
        _Anvil_xint_assign_64(&M, m);

        _Anvil_xint_mul(&D2, &M, &D_abs);
        _Anvil_xint_mul_int(&D2, 2);

        // Compare D2 with y
        int cmp_d2_y = _Anvil_xint_cmp(&D2, &y);

        if (cmp_d2_y < 0)
        {
            //printf("cmp_d2_y < 0\n");

            //DIGIT_T TWO_D2[NUM_DIGS];
            _Anvil_xint_mul_int(&D2, 2);
            int cmp_2d2_y = _Anvil_xint_cmp(&D2, &y);
            // D2 < y
            if ((m == two_to_n_minus_1) && (D_sign < 0) && (cmp_2d2_y > 0))
            {
                z = prev_float(z);
            }
            else
            {
                printf("Loops = %d\n", loop);
                return z;
            }
        }
        else if (cmp_d2_y == 0)
        {
//            printf("cmp_d2_y == 0\n");
            // If m is even
            if ((m % 2) == 0)
            {
                if ((m == two_to_n_minus_1) && (D_sign < 0))
                {
                    z = prev_float(z);
                }
                else
                {
                    printf("Loops = %d\n", loop);
                    return z;
                }
            }
            else
            {
                if (D_sign < 0)
                {
                    printf("Loops = %d\n", loop);
                    return prev_float(z);
                }
                if (D_sign > 0)
                {
                    printf("Loops = %d\n", loop);
                    return next_float(z);
                }
            }
        }
        else
        {
            //printf("cmp_d2_y > 0\n");
            if (D_sign < 0)
            {
                z = prev_float(z);
            }
            if (D_sign > 0)
            {
                z = next_float(z);
            }
        }
        ++loop;
    }
}

static const double pos_exp[] =
{
    1e0, 1e1, 1e2, 1e3, 1e4, 1e5,
    1e6, 1e7, 1e8, 1e9, 1e10,
    1e11, 1e12, 1e13, 1e14, 1e15
};
static const double bin_exp[] =
{
    1e16, 1e32, 1e64, 1e128, 1e256
};

double ten_to_e(unsigned e)
{
    double res;
    int ndx;

    if (e == 0)
    {
        return 1.0;
    }

    res = pos_exp[(e & 0xf)];
    e >>= 4;
    ndx = 0;
    while (e)
    {
        if (e & 1)
        {
            res *= bin_exp[ndx];
        }
        ++ndx;
        e >>= 1;
    }

    return res;
}

void dump_double(double z)
{
    union
    {
        double dbl;
        uint64_t uint;
    } z_z;
    z_z.dbl = z;

    uint32_t hi = (uint64_t)z_z.uint >> 32 & 0xffffffff;
    uint32_t lo = (uint64_t)z_z.uint & 0xffffffff;
    printf("%08x %08x\n", hi, lo);
}

double _Anvil_strtod(const char *restrict nptr, char **restrict endptr)
{
    const char *str = nptr;
    uint64_t mantissa;
    int exponent;
    int base;

    int neg;
    int exp_neg;
    int mantissa_exp;
    int dec_point;
    int mantissa_full;

    _Anvil_xint mant_big;

    // Skip the whitespace
    while (isspace(*str))
    {
        ++str;
    }

    // Look for the sign next
    switch (*str)
    {
        case '-':
            ++str;
            neg = 1;
            break;
        case '+':
            ++str;
            /* Fall through */
        default:
            neg = 0;
            break;
    }

    // The base can be 10 or 16
    base = 10;
    if (*nptr == '0')
    {
        if (*(nptr+1) == 'x' || *(nptr+1) == 'X')
        {
            base = 16;
            nptr += 2;;
        }
        // Todo: What if the next char isn't a hex digit
    }

    mantissa = 0;
    mantissa_full = 0;
    dec_point = 0;
    mantissa_exp = 0;

    while (1)
    {
        int digit;
        if (*str <= '9' && *str >= '0')
        {
            digit = *str - '0';
        }
        else if (base == 16 && *nptr <= 'Z' && *nptr >= 'A')
        {
            digit = *nptr - 'A' + 10;
        }
        else if (base == 16 && *nptr <= 'z' && *nptr >= 'a')
        {
            digit = *nptr - 'a' + 10;
        }
        else if (*str == '.')
        {
            dec_point = 1;
            ++str;
            continue;
        }
        else
        {
            // Non-digit, we're done
            break;
        }

        if (!dec_point)
        {
            // We haven't seen the decimal yet
            if (!mantissa_full)
            {
                uint64_t new_mantissa = mantissa * base + digit;
                if (mantissa != ((new_mantissa - digit) / base))
                {
                    // We overflowed - record the error but keep eating digits
                    mantissa_full = 1;
                    _Anvil_xint_init(&mant_big);
                    _Anvil_xint_assign_64(&mant_big, mantissa);
                    _Anvil_xint_mul_int(&mant_big, base);
                    _Anvil_xint_add_int(&mant_big, digit);
                    ++mantissa_exp;
                }
                else
                {
                    mantissa = new_mantissa;
                    //printf("mant=%llu\n", mantissa);
                }
            }
            else
            {
                _Anvil_xint_mul_int(&mant_big, base);
                _Anvil_xint_add_int(&mant_big, digit);
                ++mantissa_exp;
            }
        }
        else
        {
            if (!mantissa_full)
            {
                mantissa = mantissa * base + digit;
                --mantissa_exp;
            }
            else
            {
                _Anvil_xint_mul_int(&mant_big, 10);
                _Anvil_xint_add_int(&mant_big, digit);
            }
        }
        ++str;
    }

    exponent = 0;

    if (*str == 'e' || *str == 'E')
    {
        ++str;
        // Look for the sign first
        switch (*str)
        {
            case '-':
                ++str;
                exp_neg = 1;
                break;
            case '+':
                ++str;
                // Fall through
            default:
                exp_neg = 0;
                break;
        }

        while (1)
        {
            int digit;
            if (*str <= '9' && *str >= '0')
            {
                digit = *str - '0';
            }
            else
            {
                /* Non-digit, we're done */
                break;
            }

            int new_exponent = exponent * 10 + digit;
            if (new_exponent < exponent)
            {
                // We overflowed - record the error but keep eating digits
                mantissa_full = 1;
            }
            else
            {
                exponent = new_exponent;
            }
            ++str;
        }
        if (exp_neg)
        {
            exponent = -exponent;
        }
    }

    printf("m=%lld e=%d m=%d e+m=%d\n", mantissa, exponent, mantissa_exp, exponent + mantissa_exp);

    double estimate = mantissa;
    int total_exp = exponent + mantissa_exp;
    int ndx;
    if (total_exp > 0)
    {
        estimate *= pos_exp[(total_exp & 0xf)];
        total_exp >>= 4;
        ndx = 0;
        while (total_exp)
        {
            if (total_exp & 1)
            {
                estimate *= bin_exp[ndx];
            }
            ++ndx;
            total_exp >>= 1;
        }
    }
    else if (total_exp < 0)
    {
        total_exp = -total_exp;
        estimate /= pos_exp[(total_exp & 0xf)];
        total_exp >>= 4;
        ndx = 0;
        while (total_exp)
        {
            if (total_exp & 1)
            {
                estimate /= bin_exp[ndx];
            }
            ++ndx;
            total_exp >>= 1;
        }
    }

    if (neg)
    {
        estimate = -estimate;
    }

    if (!mantissa_full && (mantissa < two_to_n) && abs(exponent + mantissa_exp) < 15)
    {
        printf("EARLY EXIT");
        return estimate;
    }

    //dump_double(estimate);

    if (!mantissa_full)
    {
        _Anvil_xint_init(&mant_big);
        _Anvil_xint_assign_64(&mant_big, mantissa);
    }

    return algoritm_r(&mant_big, exponent, estimate);
}
