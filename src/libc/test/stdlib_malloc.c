
#include <stdlib.h>

#include "test_harness.h"

TEST_GROUP(stdlib_malloc)

TEST(stdlib_malloc, malloc)
{
    void *(* volatile MALLOC)(size_t size) = malloc;
    void (* volatile FREE)(void *ptr) = free;

    char *p[10];

    for (int i=0; i<10; ++i)
    {
        p[i] = MALLOC(150);
    }

    for (int i=0; i<10; ++i)
    {
        FREE(p[i]);
    }

    END_TEST(stdlib_malloc);
}

int stdlib_malloc_test()
{
    CALL_TEST(stdlib_malloc, malloc);

    END_TEST_GROUP(stdlib_malloc);
}
