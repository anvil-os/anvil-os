
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

TEST(string, memmove)
{
    /* MEMMOVE */
    const char orig_chars[] = "abcdefghij1234567rstuvwxyz";
    char chars[26];
    /* DEST goes from chars 10 to 16 inclusive. i.e. the digits */

    /* Call memmove thru a pointer so that gcc doesn't optimise the call away */
    void *(* volatile p_memmove)(void *s1, const void *s2, size_t n) = memmove;

    /*
     * First try a zero length memmove. We choose the case where
     * SRC (7 to 13 inclusive) begins before DEST and ends within DEST
     * because that's the one that usually fails if memcpy is used
     */
    memcpy(chars, orig_chars, 26);
    ASSERT_EQ(chars + 10, p_memmove(chars + 10, chars + 7, 0));
    ASSERT_EQ(0, memcmp(chars, orig_chars, 26));

    /* SRC (2 to 8 inclusive) begins before DEST and ends before DEST */
    memcpy(chars, orig_chars, 26);
    ASSERT_EQ(chars + 10, p_memmove(chars + 10, chars + 2, 7));
    ASSERT_EQ(0, memcmp(chars, "abcdefghijcdefghirstuvwxyz", 26));

    /* SRC (3 to 9 inclusive) begins before DEST and ends at beginning of DEST */
    memcpy(chars, orig_chars, 26);
    ASSERT_EQ(chars + 10, p_memmove(chars + 10, chars + 3, 7));
    ASSERT_EQ(0, memcmp(chars, "abcdefghijdefghijrstuvwxyz", 26));

    /* SRC (7 to 13 inclusive) begins before DEST and ends within DEST */
    memcpy(chars, orig_chars, 26);
    ASSERT_EQ(chars + 10, p_memmove(chars + 10, chars + 7, 7));
    ASSERT_EQ(0, memcmp(chars, "abcdefghijhij1234rstuvwxyz", 26));

    /* SRC (10 to 16 inclusive) coincides with DEST */
    memcpy(chars, orig_chars, 26);
    ASSERT_EQ(chars + 10, p_memmove(chars + 10, chars + 10, 7));
    ASSERT_EQ(0, memcmp(chars, "abcdefghij1234567rstuvwxyz", 26));

    /* SRC (14 to 20 inclusive) begins within DEST and ends after DEST */
    memcpy(chars, orig_chars, 26);
    ASSERT_EQ(chars + 10, p_memmove(chars + 10, chars + 14, 7));
    ASSERT_EQ(0, memcmp(chars, "abcdefghij567rsturstuvwxyz", 26));

    /* SRC (17 to 23 inclusive) begins at end of DEST and ends after DEST */
    memcpy(chars, orig_chars, 26);
    ASSERT_EQ(chars + 10, p_memmove(chars + 10, chars + 17, 7));
    ASSERT_EQ(0, memcmp(chars, "abcdefghijrstuvwxrstuvwxyz", 26));

    /* SRC (19 to 25 inclusive) begins after DEST and ends after DEST */
    memcpy(chars, orig_chars, 26);
    ASSERT_EQ(chars + 10, p_memmove(chars + 10, chars + 19, 7));
    ASSERT_EQ(0, memcmp(chars, "abcdefghijtuvwxyzrstuvwxyz", 26));

    END_TEST(string);
}

TEST(string, memchr)
{
    /* MEMCHR */
    const unsigned char mem0[] =
    {
        0x0, 0x0
    };

    const unsigned char mem1[] =
    {
        0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
        0xa8, 0xa9, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaf
    };

    /* Call memmove thru a pointer so that gcc doesn't optimise the call away */
    void *(* volatile p_memchr)(const void *s, int c, size_t n) = memchr;

    ASSERT_EQ(NULL, p_memchr(mem0, 0, 0));
    ASSERT_EQ(mem0, p_memchr(mem0, 0, 2));
    ASSERT_EQ(NULL, p_memchr(mem0, 0xa0, 2));
    ASSERT_EQ(mem1, p_memchr(mem1, 0xa0, 16));
    ASSERT_EQ(mem1 + 1, p_memchr(mem1, 0xa1, 16));
    ASSERT_EQ(mem1 + 10, p_memchr(mem1, 0xaa, 16));
    ASSERT_EQ(mem1 + 11, p_memchr(mem1+11, 0xaa, 5));
    ASSERT_EQ(mem1 + 15, p_memchr(mem1, 0xaf, 16));
    ASSERT_EQ(NULL, p_memchr(mem1, 0xaf, 15));

    END_TEST(string);
}

int string_test()
{
    CALL_TEST(string, memcmp);
    CALL_TEST(string, memset);
    CALL_TEST(string, memcpy);
    CALL_TEST(string, memmove);
    CALL_TEST(string, memchr);

    END_TEST_GROUP(string);
}