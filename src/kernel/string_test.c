
#include <stddef.h>

#include "test_harness.h"

TEST_GROUP(string)

TEST(string, fake)
{
	ASSERT_EQ(1, 1);

	END_TEST(string);
}

int string_test()
{
	CALL_TEST(string, fake);

	END_TEST_GROUP(string);
}
