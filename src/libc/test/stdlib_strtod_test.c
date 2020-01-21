
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

struct TestData base_10_parser[] =
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

TEST(stdlib_strtod, base_10_parser)
{
    double (* volatile STRTOD)(const char *restrict nptr, char **restrict endptr) = strtod;

    for (size_t i=0; i<sizeof(base_10_parser)/sizeof(base_10_parser[0]); ++i)
    {
        union
        {
            double dbl;
            uint32_t uint[2];
        } res;
        res.dbl = STRTOD(base_10_parser[i].str, NULL);
        ASSERT_EQ(base_10_parser[i].low, res.uint[0]);
        ASSERT_EQ(base_10_parser[i].high, res.uint[1]);
    }

    END_TEST(stdlib_strtod);
}

int stdlib_strtod_test()
{
    CALL_TEST(stdlib_strtod, base_10_parser);

    END_TEST_GROUP(stdlib_strtod);
}
