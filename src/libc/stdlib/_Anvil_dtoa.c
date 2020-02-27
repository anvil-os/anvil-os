
#include <stdio.h>
#include <string.h>

#include "_Anvil_double.h"
#include "_Anvil_xint.h"

char ret_str[100];

int cc0, cc1, cc2, cc3, cc4, cc5;
int aa1, aa2;

char *_Anvil_dragon4(int32_t e, uint64_t f, int32_t p, int cutoff_mode, int cutoff_place_parm, int *pk)
{
    _Anvil_xint R;
    _Anvil_xint S;
    _Anvil_xint Mplus;
    _Anvil_xint Mminus;
    _Anvil_xint TEMP;

    uint32_t U;
    int loop_cnt;
    int R5e=0, S5e=0, Mp5e=0, Mm5e=0;
    int cutoff_place = cutoff_place_parm;

    int roundup_flag = 0;
    
    char *pret_str = ret_str;
    
    int k = 0;
    
    _Anvil_xint_init(&R);
    _Anvil_xint_init(&S);
    _Anvil_xint_init(&Mplus);
    _Anvil_xint_init(&Mminus);
    _Anvil_xint_init(&TEMP);

    // NOTE: Instead of actually modifying any of the Xints we record what
    // needs to be done and calculate and optimise it all later

    // INITIALISE THE VARIABLES
    // R = f << max(e-p, 0)
    // S = 1 << max(0, -(e-p))
    // M+ = 1 << max(e-p, 0)
    // M- = 1 << max(e-p, 0)
    int R2e, S2e, Mp2e, Mm2e;
    R2e = MAX(e-p, 0);
    S2e = MAX(0, -(e-p));
    Mp2e = MAX(e-p, 0);
    Mm2e = MAX(e-p, 0);

    // FIXUP PROCEDURE
    // Account for unequal gaps
    if (f == (1U << (p-1)))
    {
        // M+ = M+ << 1
        // R = R << 1
        // S = S << 1
        ++Mp2e;
        ++R2e;
        ++S2e;
    }

    // Dragon4 has a pair of loops, one executed while R < ceil(S/10)
    // and the second while R + M+/2 >= S. The result of these is to give
    // S/10 <= R < S.
    //
    // while R < ceil(S/10)
    // {
    //    --k;
    //    R = R * 10
    //    M- = M- * 10
    //    M+ = M+ * 10
    // }
    // while (R + M+/2 >= S)
    // {
    //    S = S * 10
    //    ++k
    // }
    //
    // David M Gay in "Correctly Rounded Binary-Decimal and Decimal Binary
    // Conversion" suggests that if we calculate an approximation of the log10
    // of the input number we can speed this up.
    // As long as we err in the correct direction we can remove the need for
    // the first loop and cause the 2nd loop to be executed either 0 or 1 times
    // To that end we keep the 2nd loop inside as below.
    //
    uint64_t f1 = f;
    while ((f1 & 0x10000000000000) == 0)
    {
        f1 <<= 1;
        --e;
    }
    int log10 = (e * 30103 / 100000 + 1);
    if (log10 < 0)
    {
        --log10;
    }

    k = log10;
    if (k < 0)
    {
        R2e -= k;
        R5e -= k;
        Mp2e -= k;
        Mp5e -= k;
        Mm2e -= k;
        Mm5e -= k;
    }
    else
    {
        S5e += k;
        S2e += k;
    }

//    printf("f=%lld e=%d log=%d\n", f, e, log10);
    
    // Remove common factors now
    int lowest = 10000;
    if (R2e < lowest) lowest = R2e;
    if (S2e < lowest) lowest = S2e;
    if (Mp2e < lowest) lowest = Mp2e;
    if (Mm2e < lowest) lowest = Mm2e;
    R2e -= lowest;
    S2e -= lowest;
    Mp2e -= lowest;
    Mm2e -= lowest;
    lowest = 10000;
    if (R5e < lowest) lowest = R5e;
    if (S5e < lowest) lowest = S5e;
    if (Mp5e < lowest) lowest = Mp5e;
    if (Mm5e < lowest) lowest = Mm5e;
    R5e -= lowest;
    S5e -= lowest;
    Mp5e -= lowest;
    Mm5e -= lowest;
    
//    printf("2: %d %d %d %d\n", R2e, S2e, Mp2e, Mm2e);
//    printf("5: %d %d %d %d\n", R5e, S5e, Mp5e, Mm5e);

    _Anvil_xint_assign_64(&R, f);
    _Anvil_xint_lshift(&R, &R, R2e);
    _Anvil_xint_mul_5exp(&R, R5e);
    _Anvil_xint_assign_64(&S, 1);
    _Anvil_xint_lshift(&S, &S, S2e);
    _Anvil_xint_mul_5exp(&S, S5e);
    _Anvil_xint_assign_64(&Mplus, 1);
    _Anvil_xint_lshift(&Mplus, &Mplus, Mp2e);
    _Anvil_xint_mul_5exp(&Mplus, Mp5e);
    _Anvil_xint_assign_64(&Mminus, 1);
    _Anvil_xint_lshift(&Mminus, &Mminus, Mm2e);
    _Anvil_xint_mul_5exp(&Mminus, Mm5e);


    // while (R < ceil(S/B))
    // {
    //    --k;
    //    R = R * 10
    //    M- = M- * 10
    //    M+ = M+ * 10
    // }
    // calculate ceil(S/B)
    if (_Anvil_xint_div_int(&TEMP, &S, 10))
    {
        // If there is a remainder round up to get ceiling
        _Anvil_xint_add_int(&TEMP, 1);
    }
    // while R < ceil(S/B)
    loop_cnt = 0;
    while (_Anvil_xint_cmp(&R, &TEMP) < 0)
    {
        // k = k-1
        // R = R * B
        // M- = M- * B
        // M+ = M+ * B
        --k;
        _Anvil_xint_mul_int(&R, 10);
        ++R5e;
        ++R2e;
        _Anvil_xint_mul_int(&Mminus, 10);
        ++Mm5e;
        ++Mm2e;
        _Anvil_xint_mul_int(&Mplus, 10);
        ++Mp5e;
        ++Mp2e;
        ++loop_cnt;
        ++cc0;
    }

    //printf("Loop1=%d\n", loop_cnt);

    // In the next part of the algorithm we need to compare
    // 2*R + M+ with 2*S

    // Let TEMP = 2 * R + M+
    _Anvil_xint_lshift(&TEMP, &R, 1);
    _Anvil_xint_add(&TEMP, &Mplus);

    ///////////////////////////////////////
    // Temporarily set S to S * 2
    _Anvil_xint_lshift(&S, &S, 1);
    ///////////////////////////////////////

    int a = 0;
    do
    {
        if (a > 0)
        {
            // Pre-multiply S by 10^a
            k += a;
            _Anvil_xint_mul_5exp(&S, a);
            S5e += a;
            _Anvil_xint_lshift(&S, &S, a);
            S2e += a;
        }
        loop_cnt = 0;
        // while TEMP >= 2 * S
        while (_Anvil_xint_cmp(&TEMP, &S) >= 0)
        {
            // S = S * B
            // k = k + 1
            _Anvil_xint_mul_int(&S, 10);
            ++k;
            ++S5e;
            ++S2e;
            ++loop_cnt;
            ++cc1;
        }
//        if (loop_cnt) printf("Loop2 %d\n", loop_cnt);
        //if (a > 0 && a != loop_cnt) printf("Loops=%d a=%d\n", loop_cnt, a);
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
                a = cutoff_place - k;
                if (cutoff_mode == e_relative && a > -1)
                {
                    a = -1;
                }
                // Let's use TEMP for Y
                // Set it to S (note S is currently 2 * S)
                _Anvil_xint_rshift(&TEMP, &S, 1);
//                printf("K=%d P=%d C=%d A=%d\n", k, cutoff_place_parm, cutoff_place, a);
                if (a > 0)
                {
                    // Y = S * 10 ^ a
                    _Anvil_xint_mul_5exp(&TEMP, a);
                    _Anvil_xint_lshift(&TEMP, &TEMP, a);
                }
                else if (a < 0)
                {
                    // Y = ceil(S / (10 ^ -a))
                    int a5 = S5e + a;
                    int a2 = S2e + a;
                    _Anvil_xint_assign_64(&TEMP, 1);
                    if (a5 > 0 || a2 > 0)
                    {
                        if (a5 < 0)
                        {
//                            note = 1;
//                            printf("a=%d: a5=%d a2=%d S5e=%d S2e=%d Mp5e=%d Mp2e=%d\n", a, a5, a2, S5e, S2e, Mp5e, Mp2e);
                        }
                        if (a5 > a2)
                        {
                            _Anvil_xint_mul_5exp(&TEMP, a5);
                            _Anvil_xint_lshift(&TEMP, &TEMP, a2);
                        }
                        else
                        {
                            _Anvil_xint_lshift(&TEMP, &TEMP, a2);
                            _Anvil_xint_mul_5exp(&TEMP, a5);
                        }
                    }
                    if (_Anvil_xint_is_zero(&TEMP))
                    {
                        _Anvil_xint_assign_64(&TEMP, 1);
                    }
                }
                // If Y is greater than M use it
                if (_Anvil_xint_cmp(&TEMP, &Mminus) > 0)
                {
                    _Anvil_xint_assign(&Mminus, &TEMP);
                }
                if (_Anvil_xint_cmp(&TEMP, &Mplus) > 0)
                {
                    _Anvil_xint_assign(&Mplus, &TEMP);
                }
                if (_Anvil_xint_cmp(&TEMP, &Mplus) == 0)
                {
                    roundup_flag = 1;
                }
                // We need to set TEMP back to (2 * R + M+) here for the test
                // below
                _Anvil_xint_lshift(&TEMP, &R, 1);
                _Anvil_xint_add(&TEMP, &Mplus);
                break;
            }
        }
//        if (_Anvil_xint_cmp(&TEMP, &S) >= 0)
//        {
//            //printf("We're looping a=%d\n", a);
//        }
    } while (_Anvil_xint_cmp(&TEMP, &S) >= 0);

    ///////////////////////////////////////
    // Restore S back to being S
    _Anvil_xint_rshift(&S, &S, 1);
    ///////////////////////////////////////

    // LOOP
    int low;
    int high;

    // From now on let R actually be 2R
    _Anvil_xint_mul_int(&R, 2);
    
    // The original Dragon4 algorithm doesn't have this test but it's
    // certainly needed
    if (_Anvil_xint_cmp(&R, &Mminus) >= 0)
    {
        while (1)
        {
            ++cc4;
            --k;
            
            // U = floor ( R * 10 ) / S
            // R = ( R * 10 ) mod S
            _Anvil_xint_mul_int(&R, 5);
            
            U = _Anvil_xint_div_small(&R, &R, &S);

            // R is 2 * R as stated above
            _Anvil_xint_lshift(&R, &R, 1);

            // M+ = M+ * 10
            _Anvil_xint_mul_int(&Mplus, 10);
            
            // M- = M- * 10
            _Anvil_xint_mul_int(&Mminus, 10);
            
            // low = 2 * R < M-
            low = _Anvil_xint_cmp(&R, &Mminus) == -1;
            
            // high = 2 * R > 2 * S - M+
            _Anvil_xint_lshift(&TEMP, &S, 1);

            if (_Anvil_xint_cmp(&TEMP, &Mplus) >= 0)
            {
                _Anvil_xint_sub(&TEMP, &TEMP, &Mplus);
                // According to the Dragon logic this should be >= 1 but >= 0 works - this is the 10^23 problem
                if (roundup_flag)
                {
                    high = _Anvil_xint_cmp(&R, &TEMP) >= 0;
                }
                else
                {
                    high = _Anvil_xint_cmp(&R, &TEMP) > 0;
                }
            }
            else
            {
                high = 1;
            }

            if (low || high || (k == cutoff_place))
            {
                break;
            }
            *pret_str++ = U + 0x30;
        }

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
            int cmp = _Anvil_xint_cmp(&R, &S);
            if (cmp < 0)
            {
                *pret_str++ = U + 0x30;
            }
            else if (cmp > 0)
            {
                *pret_str++ = U + 1 + 0x30;
            }
            else
            {
                *pret_str++ = U + 0x30;
            }
        }
    }
    
    *pret_str = 0;
    *pk = k;

    _Anvil_xint_delete(&R);
    _Anvil_xint_delete(&S);
    _Anvil_xint_delete(&Mplus);
    _Anvil_xint_delete(&Mminus);
    _Anvil_xint_delete(&TEMP);

    return ret_str;
}

char *_Anvil_dtoa(double dd, int mode, int ndigits, int *decpt, int *sign, char **rve)
{
    ++cc5;

    int32_t e = 0;
    uint64_t f = 0;
    int32_t p = 0;
    int cutoff_mode = mode;
    int cutoff_place = ndigits;
    
    if (dd == 0.0)
    {
        strcpy(ret_str, "0");
        *decpt = 1;
        return ret_str;
    }

    //
    // Our double is f * 2^(e-p)
    //
    split_double(dd, sign, &f, &e);
    p = 52;

    char *ret = _Anvil_dragon4(e, f, p, cutoff_mode, -cutoff_place, decpt);
    *decpt += strlen(ret);

    return ret;
}
