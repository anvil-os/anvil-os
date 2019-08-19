
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "test_harness.h"

#pragma GCC diagnostic ignored "-Woverflow"

TEST_GROUP(stdio_printf)

TEST(stdio_printf, fmt_none)
{
    /* SPRINTF */
    int (* volatile SPRINTF)(char *restrict s, const char *restrict format, ...) = sprintf;
    char buf[100];

    struct test_point
    {
        char *output;
        int ret;
        char *fmt;
        char *arg;
    };

    struct test_point test_vector[] =
    {
        { "",             0, "",                 "xxx" },
        { "Hello world", 11, "Hello world\0etc", "xxx" },
        { 0 }
    };

    for (int i=0; test_vector[i].output; ++i)
    {
        ASSERT_EQ(test_vector[i].ret, SPRINTF(buf, test_vector[i].fmt));
        ASSERT_EQ(0, strcmp(buf, test_vector[i].output));
    }

    END_TEST(stdio_printf);
}

TEST(stdio_printf, fmt_s)
{
    /* SPRINTF */
    int (* volatile SPRINTF)(char *restrict s, const char *restrict format, ...) = sprintf;
    char buf[100];

    struct test_point
    {
        char *output;
        int ret;
        char *fmt;
        char *arg;
    };

    struct test_point test_vector[] =
    {
        { "abc",   3, "%s",     "abc"  },
        { "  abc", 5, "%5s",    "abc" },
        { "ab",    2, "%.2s",   "abc"  },
        { "   ab", 5, "%5.2s",  "abc" },
        { "abc  ", 5, "%-5s",   "abc"  },
        { "ab",    2, "%-.2s",  "abc"  },
        { "ab   ", 5, "%-5.2s", "abc" },
        { 0 }
    };

    for (int i=0; test_vector[i].output; ++i)
    {
        ASSERT_EQ(test_vector[i].ret, SPRINTF(buf, test_vector[i].fmt, test_vector[i].arg));
        ASSERT_EQ(0, strcmp(buf, test_vector[i].output));
    }
    END_TEST(stdio_printf);
}

TEST(stdio_printf, fmt_hhd)
{
    /* SPRINTF */
    int (* volatile SPRINTF)(char *restrict s, const char *restrict format, ...) = sprintf;
    char buf[100];

    struct test_point
    {
        char *output;
        int ret;
        char *fmt;
        char arg;
    };

    struct test_point test_vector[] =
    {
        { "0",     1, "%hhd",     0 },
        { "123",   3, "%hhd",     123 },
        { "-33",   3, "%hhd",     223 },
        { "-2",    2, "%hhd",     (unsigned)-2 },
        { 0 }
    };

    for (int i=0; test_vector[i].output; ++i)
    {
        ASSERT_EQ(test_vector[i].ret, SPRINTF(buf, test_vector[i].fmt, test_vector[i].arg));
        ASSERT_EQ(0, strcmp(buf, test_vector[i].output));
    }
    END_TEST(stdio_printf);
}

TEST(stdio_printf, fmt_hhu)
{
    /* SPRINTF */
    int (* volatile SPRINTF)(char *restrict s, const char *restrict format, ...) = sprintf;
    char buf[100];

    struct test_point
    {
        char *output;
        int ret;
        char *fmt;
        unsigned char arg;
    };

    struct test_point test_vector[] =
    {
        { "0",     1, "%hhu",     0 },
        { "123",   3, "%hhu",     123 },
        { "223",   3, "%hhu",     223 },
        { "254",   3, "%hhu",     (unsigned)-2 },
        { 0 }
    };

    for (int i=0; test_vector[i].output; ++i)
    {
        ASSERT_EQ(test_vector[i].ret, SPRINTF(buf, test_vector[i].fmt, test_vector[i].arg));
        ASSERT_EQ(0, strcmp(buf, test_vector[i].output));
    }
    END_TEST(stdio_printf);
}

TEST(stdio_printf, fmt_hd)
{
    /* SPRINTF */
    int (* volatile SPRINTF)(char *restrict s, const char *restrict format, ...) = sprintf;
    char buf[100];

    struct test_point
    {
        char *output;
        int ret;
        char *fmt;
        short arg;
    };

    struct test_point test_vector[] =
    {
        { "0",      1, "%hd", 0 },
        { "123",    3, "%hd", 123 },
        { "223",    3, "%hd", 223 },
        { "-2",     2, "%hd", (unsigned)-2 },
        { "-10000", 6, "%hd", (unsigned)-10000 },
        { 0 }
    };

    for (int i=0; test_vector[i].output; ++i)
    {
        ASSERT_EQ(test_vector[i].ret, SPRINTF(buf, test_vector[i].fmt, test_vector[i].arg));
        ASSERT_EQ(0, strcmp(buf, test_vector[i].output));
    }
    END_TEST(stdio_printf);
}

TEST(stdio_printf, fmt_hu)
{
    /* SPRINTF */
    int (* volatile SPRINTF)(char *restrict s, const char *restrict format, ...) = sprintf;
    char buf[100];

    struct test_point
    {
        char *output;
        int ret;
        char *fmt;
        unsigned short arg;
    };

    struct test_point test_vector[] =
    {
        { "0",     1, "%hu", 0 },
        { "123",   3, "%hu", 123 },
        { "223",   3, "%hu", 223 },
        { "65534", 5, "%hu", (unsigned)-2 },
        { "55536", 5, "%hu", (unsigned)-10000 },
        { 0 }
    };

    for (int i=0; test_vector[i].output; ++i)
    {
        ASSERT_EQ(test_vector[i].ret, SPRINTF(buf, test_vector[i].fmt, test_vector[i].arg));
        ASSERT_EQ(0, strcmp(buf, test_vector[i].output));
    }
    END_TEST(stdio_printf);
}

TEST(stdio_printf, fmt_d)
{
    /* SPRINTF */
    int (* volatile SPRINTF)(char *restrict s, const char *restrict format, ...) = sprintf;
    char buf[100];

    struct test_point
    {
        char *output;
        int ret;
        char *fmt;
        int arg;
    };

    struct test_point test_vector[] =
    {
        { "0",           1, "%d", 0 },
        { "123",         3, "%d", 123 },
        { "223",         3, "%d", 223 },
        { "33333",       5, "%d", 33333 },
        { "787878787",   9, "%d", 787878787 },
        { "-2",          2, "%d", (unsigned)-2 },
        { "-10000",      6, "%d", -10000 },
        { "-787878787", 10, "%d", -787878787 },
        { 0 }
    };

    for (int i=0; test_vector[i].output; ++i)
    {
        ASSERT_EQ(test_vector[i].ret, SPRINTF(buf, test_vector[i].fmt, test_vector[i].arg));
        ASSERT_EQ(0, strcmp(buf, test_vector[i].output));
    }
    END_TEST(stdio_printf);
}

TEST(stdio_printf, fmt_u)
{
    /* SPRINTF */
    int (* volatile SPRINTF)(char *restrict s, const char *restrict format, ...) = sprintf;
    char buf[100];

    struct test_point
    {
        char *output;
        int ret;
        char *fmt;
        unsigned arg;
    };

    struct test_point test_vector[] =
    {
        { "0",           1, "%u", 0 },
        { "123",         3, "%u", 123 },
        { "223",         3, "%u", 223 },
        { "33333",       5, "%u", 33333 },
        { "787878787",   9, "%u", 787878787 },
        { "4294967294", 10, "%u", (unsigned)-2 },
        { "4294957296", 10, "%u", -10000 },
        { "3507088509", 10, "%u", -787878787 },
        { 0 }
    };

    for (int i=0; test_vector[i].output; ++i)
    {
        ASSERT_EQ(test_vector[i].ret, SPRINTF(buf, test_vector[i].fmt, test_vector[i].arg));
        ASSERT_EQ(0, strcmp(buf, test_vector[i].output));
    }
    END_TEST(stdio_printf);
}

TEST(stdio_printf, fmt_ld)
{
    /* SPRINTF */
    int (* volatile SPRINTF)(char *restrict s, const char *restrict format, ...) = sprintf;
    char buf[100];

    struct test_point
    {
        char *output;
        int ret;
        char *fmt;
        long arg;
    };

    struct test_point test_vector[] =
    {
        { "0",           1, "%ld", 0 },
        { "123",         3, "%ld", 123 },
        { "223",         3, "%ld", 223 },
        { "33333",       5, "%ld", 33333 },
        { "787878787",   9, "%ld", 787878787 },
        { "-2",          2, "%ld", (unsigned)-2 },
        { "-10000",      6, "%ld", -10000 },
        { "-787878787", 10, "%ld", -787878787 },
        { 0 }
    };

    for (int i=0; test_vector[i].output; ++i)
    {
        ASSERT_EQ(test_vector[i].ret, SPRINTF(buf, test_vector[i].fmt, test_vector[i].arg));
        ASSERT_EQ(0, strcmp(buf, test_vector[i].output));
    }
    END_TEST(stdio_printf);
}

TEST(stdio_printf, fmt_lu)
{
    /* SPRINTF */
    int (* volatile SPRINTF)(char *restrict s, const char *restrict format, ...) = sprintf;
    char buf[100];

    struct test_point
    {
        char *output;
        int ret;
        char *fmt;
        unsigned long arg;
    };

    struct test_point test_vector[] =
    {
        { "0",           1, "%lu", 0 },
        { "123",         3, "%lu", 123 },
        { "223",         3, "%lu", 223 },
        { "33333",       5, "%lu", 33333 },
        { "787878787",   9, "%lu", 787878787 },
        { "4294967294", 10, "%lu", (unsigned)-2 },
        { "4294957296", 10, "%lu", -10000 },
        { "3507088509", 10, "%lu", -787878787 },
        { 0 }
    };

    for (int i=0; test_vector[i].output; ++i)
    {
        ASSERT_EQ(test_vector[i].ret, SPRINTF(buf, test_vector[i].fmt, test_vector[i].arg));
        ASSERT_EQ(0, strcmp(buf, test_vector[i].output));
    }
    END_TEST(stdio_printf);
}

TEST(stdio_printf, fmt_lld)
{
    /* SPRINTF */
    int (* volatile SPRINTF)(char *restrict s, const char *restrict format, ...) = sprintf;
    char buf[100];

    struct test_point
    {
        char *output;
        int ret;
        char *fmt;
        long long arg;
    };

    struct test_point test_vector[] =
    {
        { "0",           1, "%lld", 0 },
        { "123",         3, "%lld", 123 },
        { "223",         3, "%lld", 223 },
        { "33333",       5, "%lld", 33333 },
        { "787878787",   9, "%lld", 787878787 },
        { "-2",          2, "%lld", (long long)-2 },
        { "-10000",      6, "%lld", -10000LL },
        { "-787878787", 10, "%lld", -787878787LL },
        { "8446744073709551615", 19, "%lld", 8446744073709551615 },
        { 0 }
    };

    for (int i=0; test_vector[i].output; ++i)
    {
        ASSERT_EQ(test_vector[i].ret, SPRINTF(buf, test_vector[i].fmt, test_vector[i].arg));
        ASSERT_EQ(0, strcmp(buf, test_vector[i].output));
    }
    END_TEST(stdio_printf);
}

TEST(stdio_printf, fmt_llu)
{
    /* SPRINTF */
    int (* volatile SPRINTF)(char *restrict s, const char *restrict format, ...) = sprintf;
    char buf[100];

    struct test_point
    {
        char *output;
        int ret;
        char *fmt;
        unsigned long long arg;
    };

    struct test_point test_vector[] =
    {
        { "0",           1, "%llu", 0 },
        { "123",         3, "%llu", 123 },
        { "223",         3, "%llu", 223 },
        { "33333",       5, "%llu", 33333 },
        { "787878787",   9, "%llu", 787878787 },
        { "4294967294", 10, "%llu", (unsigned)-2 },
        { "18446744073709551615", 20, "%llu", 18446744073709551615U },
        { 0 }
    };

    for (int i=0; test_vector[i].output; ++i)
    {
        ASSERT_EQ(test_vector[i].ret, SPRINTF(buf, test_vector[i].fmt, test_vector[i].arg));
        ASSERT_EQ(0, strcmp(buf, test_vector[i].output));
    }
    END_TEST(stdio_printf);
}

TEST(stdio_printf, fmt_x)
{
    /* SPRINTF */
    int (* volatile SPRINTF)(char *restrict s, const char *restrict format, ...) = sprintf;
    char buf[100];

    struct test_point
    {
        char *output;
        int ret;
        char *fmt;
        unsigned arg;
    };

    struct test_point test_vector[] =
    {
        { "0",           1, "%x", 0 },
        { "12a",         3, "%x", 0x12a },
        { "ffffffff",    8, "%x", 0xffffffff },
        { "12A",         3, "%X", 0x12a },
        { "FFFFFFFF",    8, "%X", 0xffffffff },
        { "123456",      6, "%X", 0x123456 },
        { "00123456",    8, "%08X", 0x123456 },
//        { "0x123456",    8, "%#08X", 0x123456 },
        { 0 }
    };

    for (int i=0; test_vector[i].output; ++i)
    {
        ASSERT_EQ(test_vector[i].ret, SPRINTF(buf, test_vector[i].fmt, test_vector[i].arg));
        ASSERT_EQ(0, strcmp(buf, test_vector[i].output));
    }
    END_TEST(stdio_printf);
}

TEST(stdio_printf, fmt_o)
{
    /* SPRINTF */
    int (* volatile SPRINTF)(char *restrict s, const char *restrict format, ...) = sprintf;
    char buf[100];

    struct test_point
    {
        char *output;
        int ret;
        char *fmt;
        unsigned arg;
    };

    struct test_point test_vector[] =
    {
        { "0",            1, "%o", 0 },
        { "123",          3, "%o", 0123 },
        { "37777777777", 11, "%o", 0xffffffff },
        { 0 }
    };

    for (int i=0; test_vector[i].output; ++i)
    {
        ASSERT_EQ(test_vector[i].ret, SPRINTF(buf, test_vector[i].fmt, test_vector[i].arg));
        ASSERT_EQ(0, strcmp(buf, test_vector[i].output));
    }
    END_TEST(stdio_printf);
}

TEST(stdio_printf, fmt_sign)
{
    /* SPRINTF */
    int (* volatile SPRINTF)(char *restrict s, const char *restrict format, ...) = sprintf;
    char buf[100];

    struct test_point
    {
        char *output;
        int ret;
        char *fmt;
        int arg;
    };

    struct test_point test_vector[] =
    {
        { "123",       3, "%d", 123 },
        { "-123",      4, "%d", -123 },
        { " 123",      4, "% d", 123 },
        { "-123",      4, "% d", -123 },
        { "+123",      4, "%+d", 123 },
        { "-123",      4, "%+d", -123 },
        { 0 }
    };

    for (int i=0; test_vector[i].output; ++i)
    {
        ASSERT_EQ(test_vector[i].ret, SPRINTF(buf, test_vector[i].fmt, test_vector[i].arg));
        ASSERT_EQ(0, strcmp(buf, test_vector[i].output));
    }
    END_TEST(stdio_printf);
}

TEST(stdio_printf, fmt_precision)
{
    /* SPRINTF */
    int (* volatile SPRINTF)(char *restrict s, const char *restrict format, ...) = sprintf;
    char buf[100];

    struct test_point
    {
        char *output;
        int ret;
        char *fmt;
        int arg;
    };

    struct test_point test_vector[] =
    {
        { "0",  1, "%d", 0 },
        { "",   0, "%.0", 0 },
        { "      000123", 12, "%12.6d",    123 },
        { "     -000123", 12, "%12.6d",   -123 },
        { "      000123", 12, "% 12.6d",   123 },
        { "     -000123", 12, "% 12.6d",  -123 },
        { "     +000123", 12, "%+12.6d",   123 },
        { "     -000123", 12, "%+12.6d",  -123 },
        { "000123      ", 12, "%-12.6d",   123 },
        { "-000123     ", 12, "%-12.6d",  -123 },
        { " 000123     ", 12, "%- 12.6d",  123 },
        { "-000123     ", 12, "%- 12.6d", -123 },
        { "+000123     ", 12, "%-+12.6d",  123 },
        { "-000123     ", 12, "%-+12.6d", -123 },
        { 0 }
    };

    for (int i=0; test_vector[i].output; ++i)
    {
        ASSERT_EQ(test_vector[i].ret, SPRINTF(buf, test_vector[i].fmt, test_vector[i].arg));
        ASSERT_EQ(0, strcmp(buf, test_vector[i].output));
    }
    END_TEST(stdio_printf);
}

int stdio_printf_test()
{
    CALL_TEST(stdio_printf, fmt_none);
    CALL_TEST(stdio_printf, fmt_s);

    CALL_TEST(stdio_printf, fmt_hhd);
    CALL_TEST(stdio_printf, fmt_hhu);
    CALL_TEST(stdio_printf, fmt_hd);
    CALL_TEST(stdio_printf, fmt_hu);
    CALL_TEST(stdio_printf, fmt_d);
    CALL_TEST(stdio_printf, fmt_u);
    CALL_TEST(stdio_printf, fmt_ld);
    CALL_TEST(stdio_printf, fmt_lu);
    CALL_TEST(stdio_printf, fmt_lld);
    CALL_TEST(stdio_printf, fmt_llu);

    CALL_TEST(stdio_printf, fmt_x);
    CALL_TEST(stdio_printf, fmt_o);
    CALL_TEST(stdio_printf, fmt_sign);
    CALL_TEST(stdio_printf, fmt_precision);

    END_TEST_GROUP(stdio_printf);
}
