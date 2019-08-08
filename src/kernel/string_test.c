
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

TEST(string, strlen)
{
    /* STRLEN */

    /* Call strlen thru a pointer so that gcc doesn't optimise the call away */
    size_t (* volatile p_strlen)(const char *s) = strlen;

    ASSERT_EQ(0, p_strlen(""));
    ASSERT_EQ(2, p_strlen("ag"));
    ASSERT_EQ(3, p_strlen("abc"));
    ASSERT_EQ(7, p_strlen("abcdefg\0hij"));

    END_TEST(string);
}

TEST(string, strcpy)
{
    /* STRCPY */

    /* Call strcpy thru a pointer so that gcc doesn't optimise the call away */
    char *(* volatile p_strcpy)(char *restrict s1, const char *restrict s2) = strcpy;

    char dest_str[10];
    const char src_str[] = "abcdef";

    memset(dest_str, 0xcc, sizeof(dest_str));
    ASSERT_EQ(dest_str, p_strcpy(dest_str, ""));
    ASSERT_EQ(0, memcmp("\0\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc", dest_str, sizeof(dest_str)));

    memset(dest_str, 0xcc, sizeof(dest_str));
    ASSERT_EQ(dest_str, p_strcpy(dest_str, src_str));
    ASSERT_EQ(0, memcmp("abcdef\0\xcc\xcc\xcc", dest_str, sizeof(dest_str)));

    END_TEST(string);
}

TEST(string, strncpy)
{
    /* STRNCPY */

    /* Call strncpy thru a pointer so that gcc doesn't optimise the call away */
    char *(* volatile p_strncpy)(char *restrict s1, const char *restrict s2, size_t n) = strncpy;

    char dest_str[10];
    const char src_str[] = "abcdef";

    memset(dest_str, 0xcc, sizeof(dest_str));
    ASSERT_EQ(dest_str, p_strncpy(dest_str, src_str, 0));
    ASSERT_EQ(0, memcmp("\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc\xcc", dest_str, sizeof(dest_str)));

    memset(dest_str, 0xcc, sizeof(dest_str));
    ASSERT_EQ(dest_str, p_strncpy(dest_str, src_str, 5));
    ASSERT_EQ(0, memcmp("abcde\xcc\xcc\xcc\xcc\xcc", dest_str, sizeof(dest_str)));

    memset(dest_str, 0xcc, sizeof(dest_str));
    ASSERT_EQ(dest_str, p_strncpy(dest_str, src_str, 6));
    ASSERT_EQ(0, memcmp("abcdef\xcc\xcc\xcc\xcc", dest_str, sizeof(dest_str)));

    memset(dest_str, 0xcc, sizeof(dest_str));
    ASSERT_EQ(dest_str, p_strncpy(dest_str, src_str, 7));
    ASSERT_EQ(0, memcmp("abcdef\x00\xcc\xcc\xcc", dest_str, sizeof(dest_str)));

    memset(dest_str, 0xcc, sizeof(dest_str));
    ASSERT_EQ(dest_str, p_strncpy(dest_str, src_str, 9));
    ASSERT_EQ(0, memcmp("abcdef\x00\x00\x00\xcc", dest_str, sizeof(dest_str)));

    memset(dest_str, 0xcc, sizeof(dest_str));
    ASSERT_EQ(dest_str, p_strncpy(dest_str, "", 9));
    ASSERT_EQ(0, memcmp("\x00\x00\x00\x00\x00\x00\x00\x00\x00\xcc", dest_str, sizeof(dest_str)));

    END_TEST(string);
}

TEST(string, strcat)
{
    /* STRCAT */

    /* Call strcat thru a pointer so that gcc doesn't optimise the call away */
    char *(* volatile p_strcat)(char *restrict s1, const char *restrict s2) = strcat;

    char dest_str[6];
    const char src_str[] = "ab";

    /* strcat "" to "" */
    memset(dest_str, 0xcc, sizeof(dest_str));
    dest_str[0] = 0;
    ASSERT_EQ(dest_str, p_strcat(dest_str, ""));
    ASSERT_EQ(0, memcmp("\0\xcc\xcc\xcc\xcc\xcc", dest_str, sizeof(dest_str)));

    /* strcat "ab" to "" */
    memset(dest_str, 0xcc, sizeof(dest_str));
    dest_str[0] = 0;
    ASSERT_EQ(dest_str, p_strcat(dest_str, src_str));
    ASSERT_EQ(0, memcmp("ab\0\xcc\xcc\xcc", dest_str, sizeof(dest_str)));

    /* strcat "" to "cd" */
    memset(dest_str, 0xcc, sizeof(dest_str));
    dest_str[0] = 'c';
    dest_str[1] = 'd';
    dest_str[2] = 0;
    ASSERT_EQ(dest_str, p_strcat(dest_str, ""));
    ASSERT_EQ(0, memcmp("cd\0\xcc\xcc\xcc", dest_str, sizeof(dest_str)));

    /* strcat "ab" to "cd" */
    memset(dest_str, 0xcc, sizeof(dest_str));
    dest_str[0] = 'c';
    dest_str[1] = 'd';
    dest_str[2] = 0;
    ASSERT_EQ(dest_str, p_strcat(dest_str, src_str));
    ASSERT_EQ(0, memcmp("cdab\0\xcc", dest_str, sizeof(dest_str)));

    END_TEST(string);
}

TEST(string, strncat)
{
    /* STRNCAT */

    /* Call strncat thru a pointer so that gcc doesn't optimise the call away */
    char *(* volatile p_strncat)(char *restrict s1, const char *restrict s2, size_t n) = strncat;

    char dest_str[7];
    const char src_str[] = "abc";

    /* strncat ""    to "" n 0 */
    memset(dest_str, 0xcc, sizeof(dest_str));
    dest_str[0] = 0;
    ASSERT_EQ(dest_str, p_strncat(dest_str, "", 0));
    ASSERT_EQ(0, memcmp("\0\xcc\xcc\xcc\xcc\xcc\xcc", dest_str, sizeof(dest_str)));
    /* strncat ""    to "" n 2 */
    memset(dest_str, 0xcc, sizeof(dest_str));
    dest_str[0] = 0;
    ASSERT_EQ(dest_str, p_strncat(dest_str, "", 2));
    ASSERT_EQ(0, memcmp("\0\xcc\xcc\xcc\xcc\xcc\xcc", dest_str, sizeof(dest_str)));

    /* strncat "abc" to "" n 0 */
    memset(dest_str, 0xcc, sizeof(dest_str));
    dest_str[0] = 0;
    ASSERT_EQ(dest_str, p_strncat(dest_str, src_str, 0));
    ASSERT_EQ(0, memcmp("\0\xcc\xcc\xcc\xcc\xcc\xcc", dest_str, sizeof(dest_str)));
    /* strncat "abc" to "" n 2 */
    memset(dest_str, 0xcc, sizeof(dest_str));
    dest_str[0] = 0;
    ASSERT_EQ(dest_str, p_strncat(dest_str, src_str, 2));
    ASSERT_EQ(0, memcmp("ab\0\xcc\xcc\xcc\xcc", dest_str, sizeof(dest_str)));
    /* strncat "abc" to "" n 5 */
    memset(dest_str, 0xcc, sizeof(dest_str));
    dest_str[0] = 0;
    ASSERT_EQ(dest_str, p_strncat(dest_str, src_str, 5));
    ASSERT_EQ(0, memcmp("abc\0\xcc\xcc\xcc", dest_str, sizeof(dest_str)));

    /* strncat "abc" to "def" n 0 */
    memset(dest_str, 0xcc, sizeof(dest_str));
    dest_str[0] = 'd';
    dest_str[1] = 'e';
    dest_str[2] = 'f';
    dest_str[3] = 0;
    ASSERT_EQ(dest_str, p_strncat(dest_str, src_str, 0));
    ASSERT_EQ(0, memcmp("def\0\xcc\xcc\xcc", dest_str, sizeof(dest_str)));
    /* strncat "abc" to "def" n 2 */
    memset(dest_str, 0xcc, sizeof(dest_str));
    dest_str[0] = 'd';
    dest_str[1] = 'e';
    dest_str[2] = 'f';
    dest_str[3] = 0;
    ASSERT_EQ(dest_str, p_strncat(dest_str, src_str, 2));
    ASSERT_EQ(0, memcmp("defab\0\xcc", dest_str, sizeof(dest_str)));
    /* strncat "abc" to "def" n 5 */
    memset(dest_str, 0xcc, sizeof(dest_str));
    dest_str[0] = 'd';
    dest_str[1] = 'e';
    dest_str[2] = 'f';
    dest_str[3] = 0;
    ASSERT_EQ(dest_str, p_strncat(dest_str, src_str, 5));
    ASSERT_EQ(0, memcmp("defabc\0", dest_str, sizeof(dest_str)));

    /* strncat "" to "def" n 0 */
    memset(dest_str, 0xcc, sizeof(dest_str));
    dest_str[0] = 'd';
    dest_str[1] = 'e';
    dest_str[2] = 'f';
    dest_str[3] = 0;
    ASSERT_EQ(dest_str, p_strncat(dest_str, "", 0));
    ASSERT_EQ(0, memcmp("def\0\xcc\xcc\xcc", dest_str, sizeof(dest_str)));
    /* strncat "" to "def" n 2 */
    memset(dest_str, 0xcc, sizeof(dest_str));
    dest_str[0] = 'd';
    dest_str[1] = 'e';
    dest_str[2] = 'f';
    dest_str[3] = 0;
    ASSERT_EQ(dest_str, p_strncat(dest_str, "", 2));
    ASSERT_EQ(0, memcmp("def\0\xcc\xcc\xcc", dest_str, sizeof(dest_str)));
    /* strncat "" to "def" n 5 */
    memset(dest_str, 0xcc, sizeof(dest_str));
    dest_str[0] = 'd';
    dest_str[1] = 'e';
    dest_str[2] = 'f';
    dest_str[3] = 0;
    ASSERT_EQ(dest_str, p_strncat(dest_str, "", 5));
    ASSERT_EQ(0, memcmp("def\0\xcc\xcc\xcc", dest_str, sizeof(dest_str)));

    END_TEST(string);
}

TEST(string, strcmp)
{
    /* STRCMP */
    /* Call strcmp thru a pointer so that gcc doesn't optimise the call away */
    int (* volatile STRCMP)(const char *s1, const char *s2) = strcmp;

    ASSERT_EQ(0, STRCMP("", ""));
    ASSERT_EQ(1, STRCMP("abc", ""));
    ASSERT_EQ(-1, STRCMP("", "abc"));
    ASSERT_EQ(0, STRCMP("abc", "abc"));
    ASSERT_EQ(-1, STRCMP("abc", "abd"));
    ASSERT_EQ(1, STRCMP("abe", "abd"));
    ASSERT_EQ(1, STRCMP("abc", "ab"));
    ASSERT_EQ(-1, STRCMP("ab", "abc"));
    ASSERT_EQ(-1, STRCMP("ab", "ab\xaa"));
    ASSERT_EQ(-1, STRCMP("abc", "ab\xaa"));

    END_TEST(string);
}

TEST(string, strncmp)
{
    /* STRNCMP */
    /* Call strcmp thru a pointer so that gcc doesn't optimise the call away */
    int (* volatile STRNCMP)(const char *s1, const char *s2, size_t n) = strncmp;

    ASSERT_EQ(0, STRNCMP("", "", 10));
    ASSERT_EQ(1, STRNCMP("abc", "", 10));
    ASSERT_EQ(-1, STRNCMP("", "abc", 10));
    ASSERT_EQ(0, STRNCMP("abc", "abc", 10));
    ASSERT_EQ(-1, STRNCMP("abc", "abd", 10));
    ASSERT_EQ(1, STRNCMP("abe", "abd", 10));
    ASSERT_EQ(1, STRNCMP("abc", "ab", 10));
    ASSERT_EQ(-1, STRNCMP("ab", "abc", 10));
    ASSERT_EQ(-1, STRNCMP("ab", "ab\xaa", 10));
    ASSERT_EQ(-1, STRNCMP("abc", "ab\xaa", 10));
    /* testing n */
    ASSERT_EQ(0, STRNCMP("abc", "abc", 3));
    ASSERT_EQ(0, STRNCMP("abc", "abd", 2));
    ASSERT_EQ(0, STRNCMP("abc", "ab", 2));
    ASSERT_EQ(0, STRNCMP("ab", "abc", 2));
    ASSERT_EQ(0, STRNCMP("abc", "def", 0));

    END_TEST(string);
}

TEST(string, strchr)
{
    char    test_buf[100];

    /* STRCHR */
    /* Call strchr thru a pointer so that gcc doesn't optimise the call away */
    char *(* volatile STRCHR)(const char *s, int c) = strchr;

    memset(test_buf, 0, sizeof(test_buf));
    ASSERT_EQ(test_buf, STRCHR(test_buf, 0));
    ASSERT_EQ(NULL, STRCHR(test_buf, 'a'));
    strcpy(test_buf, "abb\xee""c");
    ASSERT_EQ(test_buf, STRCHR(test_buf, 'a'));
    ASSERT_EQ(test_buf + 1, STRCHR(test_buf, 'b'));
    ASSERT_EQ(test_buf + 3, STRCHR(test_buf, '\xee'));
    ASSERT_EQ(test_buf + 4, STRCHR(test_buf, 'c'));
    ASSERT_EQ(test_buf + 5, STRCHR(test_buf, 0));
    ASSERT_EQ(NULL, STRCHR(test_buf, 'd'));

    END_TEST(string);
}

TEST(string, strrchr)
{
    char    test_buf[100];
    /* STRRCHR */
    /* Call strchr thru a pointer so that gcc doesn't optimise the call away */
    char *(* volatile STRRCHR)(const char *s, int c) = strrchr;

    memset(test_buf, 0, sizeof(test_buf));
    ASSERT_EQ(test_buf, STRRCHR(test_buf, 0));
    ASSERT_EQ(NULL, STRRCHR(test_buf, 'a'));
    strcpy(test_buf, "abb\xee""c");
    ASSERT_EQ(test_buf, STRRCHR(test_buf, 'a'));
    ASSERT_EQ(test_buf + 2, STRRCHR(test_buf, 'b'));
    ASSERT_EQ(test_buf + 3, STRRCHR(test_buf, '\xee'));
    ASSERT_EQ(test_buf + 4, STRRCHR(test_buf, 'c'));
    ASSERT_EQ(test_buf + 5, STRRCHR(test_buf, 0));
    ASSERT_EQ(NULL, STRRCHR(test_buf, 'd'));

    END_TEST(string);
}

TEST(string, strspn)
{
    /* STRSPN */
    /* Call function thru a pointer so that gcc doesn't optimise the call away */
    size_t (* volatile STRSPN)(const char *s1, const char *s2) = strspn;

    ASSERT_EQ(0, STRSPN("", ""));
    ASSERT_EQ(0, STRSPN("", "x"));
    ASSERT_EQ(0, STRSPN("", "xyz"));
    ASSERT_EQ(0, STRSPN("abc", ""));
    ASSERT_EQ(0, STRSPN("abc", "x"));
    ASSERT_EQ(0, STRSPN("abc", "c"));
    ASSERT_EQ(1, STRSPN("abc", "a"));
    ASSERT_EQ(2, STRSPN("abc", "ba"));
    ASSERT_EQ(3, STRSPN("abc", "abc"));
    ASSERT_EQ(3, STRSPN("abcd", "abc"));
    ASSERT_EQ(3, STRSPN("abc", "cab"));
    ASSERT_EQ(3, STRSPN("abcd", "cab"));
    ASSERT_EQ(3, STRSPN("abc", "axbc"));
    ASSERT_EQ(3, STRSPN("abcd", "axbc"));
    ASSERT_EQ(3, STRSPN("abc", "cxab"));
    ASSERT_EQ(3, STRSPN("abcd", "cxab"));

    END_TEST(string);
}

TEST(string, strcspn)
{
    /* STRCSPN */
    /* Call function thru a pointer so that gcc doesn't optimise the call away */
    size_t (* volatile STRCSPN)(const char *s1, const char *s2) = strcspn;

    ASSERT_EQ(0, STRCSPN("", ""));
    ASSERT_EQ(0, STRCSPN("", "x"));
    ASSERT_EQ(0, STRCSPN("", "xyz"));
    ASSERT_EQ(3, STRCSPN("abc", ""));
    ASSERT_EQ(3, STRCSPN("abc", "x"));
    ASSERT_EQ(2, STRCSPN("abc", "c"));
    ASSERT_EQ(1, STRCSPN("abc", "b"));
    ASSERT_EQ(0, STRCSPN("abc", "a"));
    ASSERT_EQ(0, STRCSPN("abc", "ba"));
    ASSERT_EQ(0, STRCSPN("abc", "abc"));
    ASSERT_EQ(1, STRCSPN("abc", "bc"));
    ASSERT_EQ(0, STRCSPN("abc", "ca"));
    ASSERT_EQ(3, STRCSPN("abc", "xyz"));
    ASSERT_EQ(0, STRCSPN("abc", "ayz"));
    ASSERT_EQ(1, STRCSPN("abc", "xyb"));

    END_TEST(string);
}

TEST(string, strpbrk)
{
    /* STRPBRK */
    char *(* volatile STRPBRK)(const char *s1, const char *s2) = strpbrk;

    char test_buf[100];
    strcpy(test_buf, "");
    ASSERT_EQ(NULL, STRPBRK(test_buf, ""));
    ASSERT_EQ(NULL, STRPBRK(test_buf, "x"));
    ASSERT_EQ(NULL, STRPBRK(test_buf, "xyz"));
    strcpy(test_buf, "abc");
    ASSERT_EQ(NULL, STRPBRK(test_buf, ""));
    ASSERT_EQ(NULL, STRPBRK(test_buf, "x"));
    ASSERT_EQ(test_buf + 2, STRPBRK(test_buf, "c"));
    ASSERT_EQ(test_buf + 1, STRPBRK(test_buf, "b"));
    ASSERT_EQ(test_buf, STRPBRK(test_buf, "a"));
    ASSERT_EQ(test_buf, STRPBRK(test_buf, "ba"));
    ASSERT_EQ(test_buf, STRPBRK(test_buf, "abc"));
    ASSERT_EQ(test_buf + 1, STRPBRK(test_buf, "bc"));
    ASSERT_EQ(test_buf, STRPBRK(test_buf, "ca"));
    ASSERT_EQ(NULL, STRPBRK(test_buf, "xyz"));
    ASSERT_EQ(test_buf, STRPBRK(test_buf, "ayz"));
    ASSERT_EQ(test_buf + 1, STRPBRK(test_buf, "xyb"));

    END_TEST(string);
}

TEST(string, strstr)
{
    /* STRSPN */
    /* Call function thru a pointer so that gcc doesn't optimise the call away */
    char *(* volatile STRSTR)(const char *s1, const char *s2) = strstr;

    char test_buf[100];
    strcpy(test_buf, "");
    ASSERT_EQ(test_buf, STRSTR(test_buf, ""));
    ASSERT_EQ(NULL, STRSTR(test_buf, "a"));
    ASSERT_EQ(NULL, STRSTR(test_buf, "abc"));
    strcpy(test_buf, "aababcd");
    ASSERT_EQ(test_buf, STRSTR(test_buf, ""));
    ASSERT_EQ(test_buf, STRSTR(test_buf, "a"));
    ASSERT_EQ(test_buf, STRSTR(test_buf, "aa"));
    ASSERT_EQ(test_buf + 1, STRSTR(test_buf, "ab"));
    ASSERT_EQ(test_buf + 3, STRSTR(test_buf, "abc"));
    ASSERT_EQ(test_buf + 3, STRSTR(test_buf, "abcd"));
    ASSERT_EQ(NULL, STRSTR(test_buf, "abcde"));
    ASSERT_EQ(test_buf + 5, STRSTR(test_buf, "cd"));
    ASSERT_EQ(test_buf + 6, STRSTR(test_buf, "d"));
    ASSERT_EQ(NULL, STRSTR(test_buf, "e"));

    END_TEST(string);
}

int string_test()
{
    CALL_TEST(string, memcmp);
    CALL_TEST(string, memset);
    CALL_TEST(string, memcpy);
    CALL_TEST(string, memmove);
    CALL_TEST(string, memchr);

    CALL_TEST(string, strlen);
    CALL_TEST(string, strcpy);
    CALL_TEST(string, strncpy);
    CALL_TEST(string, strcat);
    CALL_TEST(string, strncat);
    CALL_TEST(string, strcmp);
    CALL_TEST(string, strncmp);
    CALL_TEST(string, strchr);
    CALL_TEST(string, strrchr);
    CALL_TEST(string, strspn);
    CALL_TEST(string, strcspn);
    CALL_TEST(string, strpbrk);
    CALL_TEST(string, strstr);

    END_TEST_GROUP(string);
}
