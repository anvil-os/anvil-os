
#include <stddef.h>

#include "../libc/inc/string.h"

#include "test_harness.h"

TEST_GROUP(string)

TEST(string, memcmp)
{
    /* MEMCMP */
    unsigned char mem0[] = { 0xeb, 0x99, 0xf1, 0 };
    unsigned char mem1[] = { 0xbe, 0x99, 0xf1, 0 };
    unsigned char mem2[] = { 0xbe, 0x99, 0xf1, 0 };
    unsigned char mem3[] = { 0xbe, 0x99, 0xf1, 0x3d, 0 };
    unsigned char mem4[] = { 0xbe, 0x99, 0xf1, 0x3e, 0 };

    /* Call memcmp thru a pointer so that gcc doesn't optimise the call away */
    int (* volatile p_memcmp)(const void *s1, const void *s2, size_t n) = memcmp;

    ASSERT_EQ(0, p_memcmp(mem0, mem1, 0));
    ASSERT_EQ(1, p_memcmp(mem0, mem1, 1));

    ASSERT_EQ(0, p_memcmp(mem1, mem2, 3));
    ASSERT_EQ(0, p_memcmp(mem3, mem1, 3));
    ASSERT_EQ(0, p_memcmp(mem1, mem3, 3));

    ASSERT_EQ(1, p_memcmp(mem3, mem1, 4));
    ASSERT_EQ(-1, p_memcmp(mem1, mem3, 4));

    ASSERT_EQ(-1, p_memcmp(mem3, mem4, 4));
    ASSERT_EQ(1, p_memcmp(mem4, mem3, 4));

    END_TEST(string);
}

TEST(string, memset)
{
    /* MEMSET */
    const unsigned char ch1 = '\xbe';
    const unsigned char ch2 = '\x99';

    unsigned char test_buf[100];
    int i;

    /* Call memset thru a pointer so that gcc doesn't optimise the call away */
    void *(* volatile p_memset)(void *s, int c, size_t n) = memset;

    for (i=0; i<100; ++i)
    {
        test_buf[i] = ch1;
    }

    /* Try an empty memset */
    /* We use volatile here to suppress a gcc warning */
    ASSERT_EQ(test_buf+20, p_memset(test_buf+20, ch2, (volatile int)(0)));
    for (i=0; i<100; ++i)
    {
        ASSERT_EQ(ch1, test_buf[i]);
    }

    /* Try an single byte memset */
    ASSERT_EQ(test_buf+20, p_memset(test_buf+20, ch2, 1));
    for (i=0; i<100; ++i)
    {
        if (i==20)
        {
            ASSERT_EQ(ch2, test_buf[i]);
        }
        else
        {
            ASSERT_EQ(ch1, test_buf[i]);
        }
    }

    /* Try a bigger memset */
    ASSERT_EQ(test_buf+20, p_memset(test_buf+20, ch2, 11));
    for (i=0; i<100; ++i)
    {
        if (i>=20 && i<=30)
        {
            ASSERT_EQ(ch2, test_buf[i]);
        }
        else
        {
            ASSERT_EQ(ch1, test_buf[i]);
        }
    }

    END_TEST(string);
}

TEST(string, memcpy)
{
    /* MEMCPY */
    const unsigned char ch1 = '\xbe';
    const unsigned char src[] =
    {
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9,
        0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf
    };

    unsigned char test_buf[100];
    int i;

    /* Call memcpy thru a pointer so that gcc doesn't optimise the call away */
    void *(* volatile p_memcpy)(void *restrict s1, const void *restrict s2, size_t n) = memcpy;

    /* Try a 0 length memcpy */
    memset(test_buf, ch1, sizeof(test_buf));
    ASSERT_EQ(test_buf+29, p_memcpy(test_buf+29, src, 0));
    for (i=0; i<100; ++i)
    {
        ASSERT_EQ(ch1, test_buf[i]);
    }

    /* Try a 1 byte memcpy */
    memset(test_buf, ch1, sizeof(test_buf));
    ASSERT_EQ(test_buf+29, p_memcpy(test_buf+29, src, 1));
    for (i=0; i<100; ++i)
    {
        if (i==29)
        {
            ASSERT_EQ(src[0], test_buf[i]);
        }
        else
        {
            ASSERT_EQ(ch1, test_buf[i]);
        }
    }

    /* Try a longer memcpy */
    memset(test_buf, ch1, sizeof(test_buf));
    ASSERT_EQ(test_buf+29, p_memcpy(test_buf+29, src, 13));
    for (i=0; i<100; ++i)
    {
        if (i>=29 && i<=41)
        {
            ASSERT_EQ(src[i-29], test_buf[i]);
        }
        else
        {
            ASSERT_EQ(ch1, test_buf[i]);
        }
    }

    END_TEST(string);
}

int string_test()
{
    CALL_TEST(string, memcmp);
    CALL_TEST(string, memset);
    CALL_TEST(string, memcpy);

    END_TEST_GROUP(string);
}
