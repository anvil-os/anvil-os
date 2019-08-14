
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "test_harness.h"

TEST_GROUP(stdio_printf)

TEST(stdio_printf, fmt_none)
{
    /* SPRINTF */
    int (* volatile SPRINTF)(char *restrict __s, const char *restrict __format, ...) = sprintf;
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
    int (* volatile SPRINTF)(char *restrict __s, const char *restrict __format, ...) = sprintf;
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
//        { "abc",   3, "%s",   "abc"  },
//        { "  abc", 5, "%5s",   "abc" },
//        { "abc  ", 5, "%-5s", "abc"  },
//        { "ab",    2, "%.2s", "abc"  },
//        { "   ab", 5, "%5.2s", "abc" },
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

    END_TEST_GROUP(stdio_printf);
}
