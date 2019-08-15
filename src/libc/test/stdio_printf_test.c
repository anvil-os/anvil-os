
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "test_harness.h"

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
        { "123",   3, "%hhd",     123 },
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
        { "123",   3, "%hhu",     123 },
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
        { "123",   3, "%hd",     123 },
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
        { "123",   3, "%hu",     123 },
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
        { "123",   3, "%d",     123 },
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
        { "123",   3, "%u",     123 },
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
        { "123",   3, "%ld",     123 },
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
        { "123",   3, "%lu",     123 },
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
        { "123",   3, "%lld",     123 },
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
        { "123",   3, "%llu",     123 },
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

    END_TEST_GROUP(stdio_printf);
}
