
#include <stddef.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

#include "test_harness.h"

TEST_GROUP(stdlib_strtod)

struct TestData
{
    const char *str;
    uint32_t high;
    uint32_t low;
};

struct TestData test_data[] =
{
    { "1234.5678", 0x40934a45, 0x6d5cfaad },
    { ".12345678e4", 0x40934a45, 0x6d5cfaad },
    { "12345678e-4", 0x40934a45, 0x6d5cfaad },
    { ".0012345678e6", 0x40934a45, 0x6d5cfaad },
    { "-1234.5678", 0xc0934a45, 0x6d5cfaad },
    { "-.12345678e4", 0xc0934a45, 0x6d5cfaad },
    { "-12345678e-4", 0xc0934a45, 0x6d5cfaad },
    { "-.0012345678e6", 0xc0934a45, 0x6d5cfaad },
    { "00001234.56780000", 0x40934a45, 0x6d5cfaad },
    { "0000.12345678e00004", 0x40934a45, 0x6d5cfaad },
    { "000012345678e-00004", 0x40934a45, 0x6d5cfaad },
    { "0000.0012345678e00006", 0x40934a45, 0x6d5cfaad },
    { "-00001234.56780000", 0xc0934a45, 0x6d5cfaad },
    { "-0000.12345678e00004", 0xc0934a45, 0x6d5cfaad },
    { "-000012345678e-00004", 0xc0934a45, 0x6d5cfaad },
    { "-0000.0012345678e00006", 0xc0934a45, 0x6d5cfaad },
};

TEST(stdlib_strtod, strtod)
{
    double (* volatile STRTOD)(const char *restrict nptr, char **restrict endptr) = strtod;

    for (size_t i=0; i<sizeof(test_data)/sizeof(test_data[0]); ++i)
    {
        union
        {
            double dbl;
            uint32_t uint[2];
        } res;
        printf("---- %s ----\n", test_data[i].str);
        res.dbl = STRTOD(test_data[i].str, NULL);
        //printf("%08x %08x\n", test_data[i].low, res.uint[0]);
        ASSERT_EQ(test_data[i].low, res.uint[0]);
        //printf("%08x %08x\n", test_data[i].high, res.uint[1]);
        ASSERT_EQ(test_data[i].high, res.uint[1]);
    }

    END_TEST(stdlib_strtod);
}

int stdlib_strtod_test()
{
    CALL_TEST(stdlib_strtod, strtod);

    END_TEST_GROUP(stdlib_strtod);
}
