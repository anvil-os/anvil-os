
#include <stdio.h>
#include <string.h>

#include "_Anvil_double.h"
#include "_Anvil_xint.h"

char ret_str[100];

int cc0, cc1, cc2, cc3, cc4, cc5, cc6;

static const int xint_size = 40;

char *_Anvil_dragon4(int e, uint64_t f, int p, int cutoff_mode, int cutoff_place_parm, int *pk)
{
    _Anvil_xint_mempool mempool;
    _Anvil_xint R;
    _Anvil_xint S;
    _Anvil_xint Mplus;
    _Anvil_xint _Mminus;
    _Anvil_xint *pMminus;
    _Anvil_xint TEMP;

    uint32_t U;
    int R5e=0, S5e=0, Mp5e=0, Mm5e=0;
    int R2e=0, S2e=0, Mp2e=0, Mm2e=0;
    int cutoff_place = cutoff_place_parm;

    int roundup_flag = 0;
    int unequal_gap = 0;
    
    char *pret_str = ret_str;
    
    // NOTE: Instead of actually modifying any of the Xints we record what
    // needs to be done and calculate and optimise it all later
    // Don't worry about recording values for M+ and M- since they follow
    // R early on.

    // INITIALISE THE VARIABLES
    // R = f << max(e-p, 0)
    // S = 1 << max(0, -(e-p))
    // M+ = 1 << max(e-p, 0)
    // M- = 1 << max(e-p, 0)
    if (e - p >= 0)
    {
        R2e = e - p;
    }
    else
    {
        S2e = p - e;
    }

    // FIXUP PROCEDURE
    // Account for unequal gaps
    if (f == (1U << (p-1)))
    {
        // M+ = M+ << 1
        // R = R << 1
        // S = S << 1
        unequal_gap = 1;
        ++R2e;
        ++S2e;
    }

    // Dragon4 has a pair of loops, one executed while R < ceil(S/10)
    // and the second while R + M+/2 >= S. The result of these is to give
    // S/10 <= R < S.
    //
    // David M Gay in "Correctly Rounded Binary-Decimal and Decimal Binary
    // Conversion" suggests that if we calculate an approximation of the log10
    // of the input number we can speed this up.
    // As long as we err in the correct direction we can remove the need for
    // the first loop and cause the 2nd loop to be executed either 0 or 1 times
    // We will keep both loops but expect them to be executed rarely
    //
    uint64_t f1 = f;
    int e1 = e;
    while ((f1 & 0x10000000000000) == 0)
    {
        f1 <<= 1;
        --e1;
    }
    int log10 = (e1 * 30103 / 100000 + 1);
    if (log10 < 0)
    {
        --log10;
    }

    int k = log10;
    if (k < 0)
    {
        R2e -= k;
        R5e -= k;
    }
    else
    {
        S5e += k;
        S2e += k;
    }
    
    // We need S = S / 10
    --S5e; --S2e;

    // Remove common factors now
    if (R2e < S2e)
    {
        S2e -= R2e;
        R2e = 0;
    }
    else
    {
        R2e -= S2e;
        S2e = 0;
    }
    if (R5e < S5e)
    {
        S5e -= R5e;
        R5e = 0;
    }
    else
    {
        R5e -= S5e;
        S5e = 0;
    }

    Mp5e = Mm5e = R5e;
    Mp2e = Mm2e = R2e;

    // while (R < ceil(S/10))
    // {
    //    --k;
    //    R = R * 10
    //    M- = M- * 10
    //    M+ = M+ * 10
    // }
    // calculate ceil(S/10)

    _Anvil_xint_mempool_init(&mempool, 5, xint_size);
    _Anvil_xint_init(&mempool, &R, xint_size);
    _Anvil_xint_assign_64(&R, f);
    _Anvil_xint_lshift(&R, &R, R2e);
    _Anvil_xint_mul_5exp(&R, R5e);

    _Anvil_xint_init(&mempool, &S, xint_size);
    _Anvil_xint_assign_64(&S, 1);
    _Anvil_xint_lshift(&S, &S, S2e);
    _Anvil_xint_mul_5exp(&S, S5e);

    // while R < ceil(S/B)
    while (_Anvil_xint_cmp(&R, &S) < 0)
    {
        // k = k-1
        // R = R * B
        // M- = M- * B
        // M+ = M+ * B
        ++cc2;
        --k;
        _Anvil_xint_mul_int(&R, 10);
        ++R5e; ++R2e;
        ++Mm5e; ++Mm2e;
        ++Mp5e; ++Mp2e;
    }

    _Anvil_xint_mul_int(&S, 10);
    ++S5e; ++S2e;

    //printf("Loop1=%d\n", loop_cnt);

    // In the next part of the algorithm we need to compare
    // 2*R + M+ with 2*S

    _Anvil_xint_init(&mempool, &Mplus, xint_size);
    _Anvil_xint_assign_64(&Mplus, 1);
    _Anvil_xint_lshift(&Mplus, &Mplus, Mp2e);
    _Anvil_xint_mul_5exp(&Mplus, Mp5e);

    if (unequal_gap)
    {
        _Anvil_xint_init(&mempool, &_Mminus, xint_size);
        _Anvil_xint_assign_64(&_Mminus, 1);
        _Anvil_xint_lshift(&_Mminus, &_Mminus, Mm2e);
        _Anvil_xint_mul_5exp(&_Mminus, Mm5e);
    }
    else
    {
        pMminus = &Mplus;
    }

    // Let TEMP = 2 * R + M+
    _Anvil_xint_init(&mempool, &TEMP, xint_size);
    _Anvil_xint_lshift(&TEMP, &R, 1);
    _Anvil_xint_add(&TEMP, &Mplus);

    ///////////////////////////////////////
    // Temporarily set S to S * 2
    _Anvil_xint_lshift(&S, &S, 1);
    ///////////////////////////////////////

    int loop = 0;
    do
    {
        // while TEMP >= 2 * S
        while (_Anvil_xint_cmp(&TEMP, &S) >= 0)
        {
            // S = S * B
            // k = k + 1
            ++cc3;
            _Anvil_xint_mul_int(&S, 10);
            ++k;
            ++S5e;
            ++S2e;
        }

        switch (cutoff_mode)
        {
            case e_normal:
                cutoff_place = k;
                break;
            case e_relative:
                cutoff_place = cutoff_place_parm + k;
                // no break
            case e_absolute:
            {
                // This is the Dragon4 CutoffAdjust
                int a = cutoff_place - k;
                if (cutoff_mode == e_relative && a > -1)
                {
                    a = -1;
                }
                if (cutoff_mode == e_absolute && log10 < cutoff_place_parm)
                {
                    k += a;
                    a = 0;
                }

                // Let's use TEMP for Y
                // Set Y to S (note S is currently 2 * S)
                _Anvil_xint_rshift(&TEMP, &S, 1);

                //printf("K=%d P=%d C=%d A=%d\n", k, cutoff_place_parm, cutoff_place, a);
                // We want Y = ceil(S x 10^a) = ceil(2^S2e * 5^S5e x 2^a * 5^e)
                int Y5e = S5e + a;
                int Y2e = S2e + a;

                // Y = 5^a5 * 2^a2
                // M = 5^M5e * 2^M2e
                // log2(5) = 2.321928094887362
                int yy = Y2e + Y5e * 23219 / 10000;
                int mm = Mp2e + Mp5e * 23219 / 10000;

                if (yy > mm)
                {
                    if (a > 0)
                    {
                        // Y = S * 10 ^ a
                        _Anvil_xint_mul_5exp(&TEMP, a);
                        _Anvil_xint_lshift(&TEMP, &TEMP, a);
                        ++cc4;
                    }
                    else if (a < 0)
                    {
                        // Y = ceil(S / (10 ^ -a))
                        _Anvil_xint_assign_64(&TEMP, 1);
                        if (Y5e > 0 || Y2e > 0)
                        {
                            if (Y5e > Y2e)
                            {
                                _Anvil_xint_mul_5exp(&TEMP, Y5e);
                                _Anvil_xint_lshift(&TEMP, &TEMP, Y2e);
                            }
                            else
                            {
                                _Anvil_xint_lshift(&TEMP, &TEMP, Y2e);
                                _Anvil_xint_mul_5exp(&TEMP, Y5e);
                            }
                        }
                        if (_Anvil_xint_is_zero(&TEMP))
                        {
                            _Anvil_xint_assign_64(&TEMP, 1);
                        }
                        ++cc5;
                    }
                    // If Y is greater than M use it
                    if (_Anvil_xint_cmp(&TEMP, &Mplus) > 0)
                    {
                        _Anvil_xint_assign(&Mplus, &TEMP);
                        Mp2e = S2e + a;
                        Mp5e = S5e + a;
                        ++cc6;
                    }
                    if (pMminus != &Mplus)
                    {
                        if (_Anvil_xint_cmp(&TEMP, pMminus) > 0)
                        {
                            _Anvil_xint_assign(pMminus, &TEMP);
                            Mm2e = S2e + a;
                            Mm5e = S5e + a;
                        }
                    }
                    if (_Anvil_xint_cmp(&TEMP, &Mplus) == 0)
                    {
                        roundup_flag = 1;
                    }
                }
//                printf("4: Mp2e=%d Mp5e=%d Mm2e=%d Mm5e=%d\n", Mp2e, Mp5e, Mm2e, Mm5e);
                // We need to set TEMP back to (2 * R + M+) here for the test
                // below
                _Anvil_xint_lshift(&TEMP, &R, 1);
                _Anvil_xint_add(&TEMP, &Mplus);
                break;
            }
        }
        loop = 0;
        if (_Anvil_xint_cmp(&TEMP, &S) >= 0)
        {
            loop = 1;
            //printf("We're looping\n");
        }
    } while (loop);

    ++S2e;
#if 0
    // Check all our variables are correct
    _Anvil_xint_assign_64(&TEMP, f);
    _Anvil_xint_lshift(&TEMP, &TEMP, R2e);
    _Anvil_xint_mul_5exp(&TEMP, R5e);
    if (_Anvil_xint_cmp(&TEMP, &R) != 0)
    {
        printf("R wrong\n");
    }
    _Anvil_xint_assign_64(&TEMP, 1);
    _Anvil_xint_lshift(&TEMP, &TEMP, S2e);
    _Anvil_xint_mul_5exp(&TEMP, S5e);
    if (_Anvil_xint_cmp(&TEMP, &S) != 0)
    {
        printf("S wrong\n");
    }
    _Anvil_xint_assign_64(&TEMP, 1);
    if (Mp5e > Mp2e)
    {
        _Anvil_xint_mul_5exp(&TEMP, Mp5e);
        _Anvil_xint_lshift(&TEMP, &TEMP, Mp2e);
    }
    else
    {
        _Anvil_xint_lshift(&TEMP, &TEMP, Mp2e);
        _Anvil_xint_mul_5exp(&TEMP, Mp5e);
    }
    if (_Anvil_xint_cmp(&TEMP, &Mplus) != 0)
    {
        _Anvil_xint_print("S   ", &S);
        _Anvil_xint_print("TEMP", &TEMP);
        _Anvil_xint_print("M+  ", &Mplus);
        printf("M+ wrong\n");
    }
    _Anvil_xint_assign_64(&TEMP, 1);
    if (Mm5e > Mm2e)
    {
        _Anvil_xint_mul_5exp(&TEMP, Mm5e);
        _Anvil_xint_lshift(&TEMP, &TEMP, Mm2e);
    }
    else
    {
        _Anvil_xint_lshift(&TEMP, &TEMP, Mm2e);
        _Anvil_xint_mul_5exp(&TEMP, Mm5e);
    }
    if (_Anvil_xint_cmp(&TEMP, &Mminus) != 0)
    {
        _Anvil_xint_print("S   ", &S);
        _Anvil_xint_print("TEMP", &TEMP);
        _Anvil_xint_print("M-  ", &Mminus);
        printf("M- wrong\n");
    }
#endif

    if (R2e && S2e)
    {
        //printf("x2x %d %d\n", R2e, S2e);
    }
    if (R5e && S5e)
    {
        //printf("x5x %d %d\n", R5e, S5e);
    }

    // LOOP
    // S already holds 2S so let's make R be 2R
    _Anvil_xint_lshift(&R, &R, 1);

    // The original Dragon4 algorithm doesn't have this test but it's
    // certainly needed
    if (_Anvil_xint_cmp(&R, pMminus) >= 0)
    {
        // The 'div_small' algorithm requires the denominator to have the top
        // bit in its top word set. Scale every thing to make this true
        int hbit = _Anvil_xint_highest_bit(&S) % 32;
        _Anvil_xint_lshift(&R, &R, 31-hbit);
        _Anvil_xint_lshift(&S, &S, 31-hbit);
        _Anvil_xint_lshift(&Mplus, &Mplus, 31-hbit);
        if (pMminus != &Mplus)
        {
            _Anvil_xint_lshift(pMminus, pMminus, 31-hbit);
        }

        int low;
        int high;
        int got_non_zero = 0;

        while (1)
        {
            ++cc1;
            --k;

            // U = floor ( R * 10 ) / S
            // R = ( R * 10 ) mod S
            _Anvil_xint_mul_int(&R, 10);

            U = _Anvil_xint_div_small(&R, &S);
            
            // M+ = M+ * 10
            _Anvil_xint_mul_int(&Mplus, 10);
            
            // M- = M- * 10
            if (pMminus != &Mplus)
            {
                _Anvil_xint_mul_int(pMminus, 10);
            }
            
            // low = 2 * R < M-
            low = _Anvil_xint_cmp(&R, pMminus) == -1;
            
            // high = 2 * R + M+ > 2 * S
            _Anvil_xint_assign(&TEMP, &R);
            _Anvil_xint_add(&TEMP, &Mplus);

            // According to the Dragon logic this should be >= 1 but >= 0 works - this is the 10^23 problem
            if (roundup_flag)
            {
                high = _Anvil_xint_cmp(&TEMP, &S) >= 0;
            }
            else
            {
                high = _Anvil_xint_cmp(&TEMP, &S) > 0;
            }

            if (low || high || (k == cutoff_place))
            {
                break;
            }
            if (U)
            {
                got_non_zero = 1;
            }
            if (got_non_zero)
            {
                *pret_str++ = U + 0x30;
            }
        }

        // The loop is done so output the final digit
        if (low && !high)
        {
            *pret_str++ = U + 0x30;
        }
        else if (!low && high)
        {
            *pret_str++ = U + 1 + 0x30;
        }
        else
        {
            // Scale R up by 2
            _Anvil_xint_lshift(&R, &R, 1);
            int cmp = _Anvil_xint_cmp(&R, &S);
            if (cmp <= 0)
            {
                *pret_str++ = U + 0x30;
            }
            else
            {
                *pret_str++ = U + 1 + 0x30;
            }
        }
    }

    *pret_str = 0;
    *pk = k;

    _Anvil_xint_delete(&R);
    _Anvil_xint_delete(&S);
    _Anvil_xint_delete(&Mplus);
    if (pMminus != &Mplus)
    {
        _Anvil_xint_delete(pMminus);
    }
    _Anvil_xint_delete(&TEMP);
    _Anvil_xint_mempool_free(&mempool);

    return ret_str;
}

char *_Anvil_dtoa(double dd, int mode, int ndigits, int *decpt, int *sign, char **rve)
{
    ++cc0;

    int e = 0;
    uint64_t f = 0;
    int32_t p = 0;
    int cutoff_mode = mode;
    int cutoff_place = ndigits;
    
    //
    // Our double is f * 2^(e-p)
    //
    split_double(dd, sign, &f, &e);
    p = 52;

    if (f == 0)
    {
        strcpy(ret_str, "0");
        *decpt = 1;
        return ret_str;
    }

    char *ret = _Anvil_dragon4(e, f, p, cutoff_mode, -cutoff_place, decpt);
    *decpt += strlen(ret);

    return ret;
}
