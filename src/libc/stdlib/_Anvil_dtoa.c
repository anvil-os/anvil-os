
#include <stdio.h>
#include <string.h>

#include "_Anvil_double.h"
#include "_Anvil_xint.h"

char ret_str[100];

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
    // needs to be done and optimise it all later

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

    k = 0;

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
        --k;
        _Anvil_xint_mul_int(&R, 10);
        ++loop_cnt;
    }
    if (loop_cnt)
    {
        // Scale the M's the same as R
        // M- = M- * B
        // M+ = M+ * B
        _Anvil_xint_mul_5exp(&Mminus, loop_cnt);
        _Anvil_xint_lshift(&Mminus, &Mminus, loop_cnt);
        _Anvil_xint_mul_5exp(&Mplus, loop_cnt);
        _Anvil_xint_lshift(&Mplus, &Mplus, loop_cnt);
    }

    // In the next part of the algorithm we need to compare
    // 2*R + M+ with 2*S

    // Let TEMP = 2 * R + M+
    _Anvil_xint_lshift(&TEMP, &R, 1);
    _Anvil_xint_add(&TEMP, &Mplus);

    ///////////////////////////////////////
    // Temporarily set S to S * 2
    _Anvil_xint_lshift(&S, &S, 1);
    ///////////////////////////////////////

    do
    {
        // while TEMP >= 2 * S
        while (_Anvil_xint_cmp(&TEMP, &S) >= 0)
        {
            // S = S * B
            // k = k + 1
            _Anvil_xint_mul_int(&S, 10);
            ++k;
        }
        switch (cutoff_mode)
        {
            case e_normal:
                cutoff_place = k;
                break;
            case e_relative:
                cutoff_place = cutoff_place_parm + k;
            case e_absolute:
            {
                int a = cutoff_place - k;
                if (cutoff_mode == e_relative && a > -1)
                {
                    a = -1;
                }
                _Anvil_xint Y;
                _Anvil_xint_init(&Y);
                // Set Y to S (note S is currently 2 * S)
                _Anvil_xint_div_int(&Y, &S, 2);
//                printf("K=%d P=%d C=%d A=%d\n", k, cutoff_place_parm, cutoff_place, a);
                if (a > 0)
                {
                    for (int i=0; i<a; ++i)
                    {
                        _Anvil_xint_mul_int(&Y, 10);
                    }
                }
                else
                {
                    for (int i=0; i<-a; ++i)
                    {
                        int rem = _Anvil_xint_div_int(&Y, &Y, 10);
                        if (rem)
                        {
                            _Anvil_xint_add_int(&Y, 1);
                        }
                    }
                }
//                _Anvil_xint_print("Y", &Y);
//                _Anvil_xint_print("Mminus", &Mminus);
//                _Anvil_xint_print("Mplus", &Mplus);
                if (_Anvil_xint_cmp(&Y, &Mminus) > 0)
                {
                    _Anvil_xint_assign(&Mminus, &Y);
//                    printf("Adjust M-\n");
                }
                if (_Anvil_xint_cmp(&Y, &Mplus) > 0)
                {
                    _Anvil_xint_assign(&Mplus, &Y);
//                    printf("Adjust M+\n");
                }
                if (_Anvil_xint_cmp(&Y, &Mplus) == 0)
                {
                    roundup_flag = 1;
                }
//                _Anvil_xint_print("Y", &Y);
//                _Anvil_xint_print("Mminus", &Mminus);
//                _Anvil_xint_print("Mplus", &Mplus);
                _Anvil_xint_delete(&Y);
                // NOTE: !!!!
                // We need to recalculate TEMP here if M+ changed
                _Anvil_xint_lshift(&TEMP, &R, 1);
                _Anvil_xint_add(&TEMP, &Mplus);
                break;
            }
        }
    } while (_Anvil_xint_cmp(&TEMP, &S) >= 0);

    ///////////////////////////////////////
    // Restore S back to being S
    _Anvil_xint_div_int(&S, &S, 2);
    ///////////////////////////////////////

    // LOOP
    int low;
    int high;
    
//    _Anvil_xint_print("R", &R);
//    _Anvil_xint_print("S", &S);
//    _Anvil_xint_print("M+", &Mplus);
//    _Anvil_xint_print("M-", &Mminus);

    // From now on let R actually be 2R
    _Anvil_xint_mul_int(&R, 2);
    
    // The original Dragon4 algorithm doesn't have this test but it's
    // certainly needed
    if (_Anvil_xint_cmp(&R, &Mminus) == -1)
    {
        //printf("ALREADY LOW k=%d\n", k);
        *pret_str = 0;
        *pk = k;

        _Anvil_xint_delete(&R);
        _Anvil_xint_delete(&S);
        _Anvil_xint_delete(&Mplus);
        _Anvil_xint_delete(&Mminus);
        _Anvil_xint_delete(&TEMP);

        return ret_str;
    }

    
    while (1)
    {
        --k;
        
        // U = floor ( R * 10 ) / S
        // R = ( R * 10 ) mod S
        _Anvil_xint_mul_int(&R, 5);
        U = _Anvil_xint_div(&R, &R, &S);

        // R is 2 * R as stated above
        _Anvil_xint_mul_int(&R, 2);

        // M+ = M+ * 10
        _Anvil_xint_mul_int(&Mplus, 10);
        
        // M- = M- * 10
        _Anvil_xint_mul_int(&Mminus, 10);
        
        // low = 2 * R < M-
        low = _Anvil_xint_cmp(&R, &Mminus) == -1;
        
        // high = 2 * R > 2 * S - M+
        _Anvil_xint_assign(&TEMP, &S);
        _Anvil_xint_mul_int(&TEMP, 2);

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

        //printf("Done\n");
        if (low || high || (k == cutoff_place))
        {
            break;
        }
        *pret_str++ = U + 0x30;
    }

//    printf("lo=%d hi=%d k=%d cut=%d r=%d\n", low, high, k, cutoff_place, roundup_flag);

    if (low && !high)
    {
        //printf("Low %d %d\n", low, high);
        *pret_str++ = U + 0x30;
    }
    else if (!low && high)
    {
        //printf("High %d %d\n", low, high);
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
        //printf("Both %d %d %d\n", low, high, cmp);
    }
    *pret_str = 0;
//    printf("strlen=%d\n", strlen(ret_str));
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

    split_double(dd, sign, &f, &e);
    p = 52;
    
    char *ret = _Anvil_dragon4(e, f, p, cutoff_mode, -cutoff_place, decpt);
    *decpt += strlen(ret);
    if ((strlen(ret) == 1) && *ret == '0')
    {
//        *ret = 0;
    }
    return ret;
}
