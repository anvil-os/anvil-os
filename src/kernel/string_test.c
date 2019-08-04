
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

int string_test()
{
    CALL_TEST(string, memcmp);

    END_TEST_GROUP(string);
}
