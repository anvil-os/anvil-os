
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "test_harness.h"

#pragma GCC diagnostic ignored "-Woverflow"

TEST_GROUP(stdio_io)

TEST(stdio_io, fputc)
{
    fputc('X', stdout);

    END_TEST(stdio_io);
}

int stdio_io_test()
{
    CALL_TEST(stdio_io, fputc);

    END_TEST_GROUP(stdio_io);
}
