
#include <stdio.h>
#include <string.h>

#include "_Anvil_double.h"
#include "_Anvil_xint.h"

char ret_str[100];

char *_Anvil_dragon4(int32_t e, uint64_t f, int32_t p, int cutoff_mode, int cutoff_place, int *pk)
{
    _Anvil_xint R;
    _Anvil_xint S;
    _Anvil_xint Mplus;
    _Anvil_xint Mminus;
    _Anvil_xint TEMP;
    uint32_t U;
    int loop_cnt;
    
    int roundup_flag = 0;
    
    char *pret_str = ret_str;
    
    int k = 0;
    
    _Anvil_xint_init(&R);
    _Anvil_xint_init(&S);
    _Anvil_xint_init(&Mplus);
    _Anvil_xint_init(&Mminus);
    _Anvil_xint_init(&TEMP);

    // INITIALISE THE VARIABLES
    // R = f << max(e-p, 0)
    _Anvil_xint_assign_64(&R, f);
    _Anvil_xint_lshift(&R, &R, MAX(e-p, 0));

    // S = 1 << max(0, -(e-p))
    _Anvil_xint_assign_64(&S, 1);
    _Anvil_xint_lshift(&S, &S, MAX(0, -(e-p)));

    // M+ = 1 << max(e-p, 0)
    _Anvil_xint_assign_64(&Mplus, 1);
    _Anvil_xint_lshift(&Mplus, &Mplus, MAX(e-p, 0));

    // M- = 1 << max(e-p, 0)
    _Anvil_xint_assign(&Mminus, &Mplus);

//    _Anvil_xint_print("R", &R);
//    _Anvil_xint_print("S", &S);
//    _Anvil_xint_print("M+", &Mplus);
//    _Anvil_xint_print("M-", &Mminus);
    //_Anvil_xint_printf(&R);

    // FIXUP
    if (f == (1U << (p-1)))
    {
        // NYI
        printf("FIXUP gap\n");
    }

    k = 0;

    // calculate ceil(S/B)
    //printf("DIV\n");
    if (_Anvil_xint_div_int(&TEMP, &S, 10))
    {
        // If there is a remainder round up to get ceiling
        _Anvil_xint_add_int(&TEMP, 1);
    }

    // while R < ceil(S/B)
    loop_cnt = 0;
//    _Anvil_xint_print("TEMP", &TEMP);
//    _Anvil_xint_print("R: ", &R);
    while (_Anvil_xint_cmp(&R, &TEMP) < 0)
    {
        // k = k-1
        --k;
        // R = R * B
        _Anvil_xint_mul_int(&R, 10);
        ++loop_cnt;
//        _Anvil_xint_print("R", &R);
    }

    if (loop_cnt)
    {
        // Scale the M's the same as R
        // M- = M- * B
        _Anvil_xint_mul_5exp(&Mminus, loop_cnt);
        _Anvil_xint_lshift(&Mminus, &Mminus, loop_cnt);
        // M+ = M+ * B
        _Anvil_xint_mul_5exp(&Mplus, loop_cnt);
        _Anvil_xint_lshift(&Mplus, &Mplus, loop_cnt);
    }

    // TEMP = 2 * R + M+
//    _Anvil_xint_print("R", &R);
    _Anvil_xint_lshift(&TEMP, &R, 1);
//    _Anvil_xint_print("TEMP", &TEMP);
//    _Anvil_xint_print("Mplus", &Mplus);
    _Anvil_xint_add(&TEMP, &Mplus);
//    _Anvil_xint_print("2R+M+", &TEMP);

    ///////////////////////////////////////
    // Temporarily set S to S * 2
    _Anvil_xint_lshift(&S, &S, 1);
    ///////////////////////////////////////

//    _Anvil_xint_print("S", &S);

    // while TEMP >= 2 * S
    while (_Anvil_xint_cmp(&TEMP, &S) >= 0)
    {
        // S = S * B
        _Anvil_xint_mul_int(&S, 10);
//        _Anvil_xint_print("S", &S);

        // k = k + 1
        ++k;
        
        switch (cutoff_mode)
        {
            case e_normal:
                cutoff_place = k;
                break;
            case e_absolute:
                // NYI
                //roundup_flag = cutoff_adjust(&S, &Mplus, &Mminus, cutoff_place, k);
                break;
            case e_relative:
                // NYI
                //cutoff_place += k;
                //roundup_flag = cutoff_adjust(&S, &Mplus, &Mminus, cutoff_place, k);
                break;
        }
    }

    ///////////////////////////////////////
    // Restore S back to being S
    _Anvil_xint_div_int(&S, &S, 2);
    ///////////////////////////////////////

    // LOOP
    int low;
    int high;
    
//    printf("READY\n");
//    _Anvil_xint_print("S", &S);
//    _Anvil_xint_print("R", &R);
//    printf("-----\n");

    // From now on let R actually be 2R
    _Anvil_xint_mul_int(&R, 2);
    
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
            high = _Anvil_xint_cmp(&R, &TEMP) >= 1;
        }
        else
        {
            high = 1;
        }

        //printf("Done\n");
        if (low || high || k == cutoff_place)
        {
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
            *pk = k;
            break;
        }
        *pret_str++ = U + 0x30;
    }

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
    int cutoff_mode = 0;
    int cutoff_place = 0;
    
    if (dd == 0.0)
    {
        return "0";
    }

    split_double(dd, sign, &f, &e);
    p = 52;

    char *ret = _Anvil_dragon4(e, f, p, cutoff_mode, cutoff_place, decpt);
    //*decpt += strlen(ret);
    return ret;
}
