
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "test_harness.h"
#include "double_test_data.h"

extern char *_Anvil_dtoa(double dd, int mode, int ndigits, int *decpt, int *sign, char **rve);

TEST_GROUP(stdlib_strtod)

TEST(stdlib_strtod, base_10_parser_data)
{
    double (* volatile STRTOD)(const char *restrict nptr, char **restrict endptr) = strtod;

    size_t i = 0;
    while (base_10_parser_data[i].str)
    {
        union
        {
            double dbl;
            uint32_t uint[2];
        } res;
        //printf("---- %s ----\n", base_10_parser_data[i].str);
        res.dbl = STRTOD(base_10_parser_data[i].str, NULL);
        ASSERT_EQ(base_10_parser_data[i].low, res.uint[0]);
        ASSERT_EQ(base_10_parser_data[i].high, res.uint[1]);
        ++i;
    }

    END_TEST(stdlib_strtod);
}

TEST(stdlib_strtod, gdtoa_test_data)
{
    double (* volatile STRTOD)(const char *restrict nptr, char **restrict endptr) = strtod;

    size_t i = 0;
    while (gdtoa_test_data[i].str)
    {
        union
        {
            double dbl;
            uint32_t uint[2];
        } res;
        //printf("---- %s ----\n", gdtoa_test_data[i].str);
        res.dbl = STRTOD(gdtoa_test_data[i].str, NULL);

        //printf("%08x %08x\n", gdtoa_test[i].low, res.uint[0]);
        //printf("%08x %08x\n", gdtoa_test[i].high, res.uint[1]);

        ASSERT_EQ(gdtoa_test_data[i].low, res.uint[0]);
        ASSERT_EQ(gdtoa_test_data[i].high, res.uint[1]);
        ++i;
    }

    END_TEST(stdlib_strtod);
}

TEST(stdlib_strtod, dtoa_test)
{
    size_t i = 0;
    while (gdtoa_test_data[i].str)
    {
        union
        {
            double dbl;
            uint32_t uint[2];
        } res;

        int decpt;
        int sign;
        char *rve;
        res.uint[0] = gdtoa_test_data[i].low;
        res.uint[1] = gdtoa_test_data[i].high;
        char *pstr = _Anvil_dtoa(res.dbl, 0, 0, &decpt, &sign, &rve);
        ++i;
    }

    END_TEST(stdlib_strtod);
}

int stdlib_strtod_test()
{
    CALL_TEST(stdlib_strtod, base_10_parser_data);
    CALL_TEST(stdlib_strtod, gdtoa_test_data);
    CALL_TEST(stdlib_strtod, dtoa_test);

    END_TEST_GROUP(stdlib_strtod);
}
