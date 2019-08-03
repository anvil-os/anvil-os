
#include <stddef.h>

#include "../libc/inc/string.h"

#include "test_harness.h"

TEST_GROUP(string)

TEST(string, memcmp)
{
    END_TEST(string);
}

int string_test()
{
    CALL_TEST(string, memcmp);

    END_TEST_GROUP(string);
}
