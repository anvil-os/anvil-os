
#include <stdlib.h>

#include "test_harness.h"

TEST_GROUP(stdlib_malloc)

size_t sizeof_block(void *p)
{
    // Cast to a size_t and step back
    size_t *pp = (size_t *)p;
    return *--pp & ~0x7;
}

TEST(stdlib_malloc, malloc1)
{
    void *(* volatile MALLOC)(size_t size) = malloc;
    void (* volatile FREE)(void *ptr) = free;

    char *p[10];

    for (int i=0; i<10; ++i)
    {
        p[i] = MALLOC(50);
        ASSERT_NE(-1, heap_check());
    }

    FREE(p[3]);
    FREE(p[7]);
    FREE(p[6]);
    FREE(p[8]);
    FREE(p[9]);
    FREE(p[1]);
    FREE(p[2]);
    FREE(p[4]);
    FREE(p[0]);
    FREE(p[5]);
    ASSERT_NE(-1, heap_check());

    END_TEST(stdlib_malloc);
}

TEST(stdlib_malloc, malloc2)
{
    void *(* volatile MALLOC)(size_t size) = malloc;
    void (* volatile FREE)(void *ptr) = free;

    char *p[100];
    for (int i=0; i<100; ++i)
    {
        char *ptr = MALLOC(50);
        ASSERT_NE(-1, heap_check());
        if (ptr)
        {
            p[i] = ptr;
        }
        else
        {
            p[i] = NULL;
        }
    }

    for (int i=0; i<100; ++i)
    {
        if (p[i])
        {
            ASSERT_NE(-1, heap_check());
            FREE(p[i]);
        }
    }

    END_TEST(stdlib_malloc);
}

TEST(stdlib_malloc, realloc)
{
    void *(* volatile MALLOC)(size_t size) = malloc;
    void (* volatile FREE)(void *ptr) = free;
    void *(* volatile REALLOC)(void *ptr, size_t size) = realloc;

    size_t *p1;
    size_t *p2;
    size_t *p3;

    // Allocate 2 blocks in succession. If we ask for 124 we should get 128
    p1 = MALLOC(124);
    ASSERT_EQ(128, sizeof_block(p1));
    p2 = MALLOC(124);
    ASSERT_EQ(128, sizeof_block(p2));
    ASSERT_NE(-1, heap_check());

    // Reducing by 1 byte should do nothing
    p3 = REALLOC(p1, 123);
    ASSERT_EQ(128, sizeof_block(p3));
    ASSERT_PTR_EQ(p3, p1);
    ASSERT_NE(-1, heap_check());

    // Reducing by 15 bytes should still do nothing
    p3 = REALLOC(p1, 109);
    ASSERT_EQ(128, sizeof_block(p3));
    ASSERT_PTR_EQ(p3, p1);
    ASSERT_NE(-1, heap_check());

    // Reducing by 16 bytes - a full block will be cut off
    p3 = REALLOC(p1, 108);
    ASSERT_EQ(112, sizeof_block(p3));
    ASSERT_PTR_EQ(p3, p1);
    ASSERT_NE(-1, heap_check());

    // Reducing by another 16 bytes - a second block will merge with the other
    p3 = REALLOC(p1, 92);
    ASSERT_EQ(96, sizeof_block(p3));
    ASSERT_PTR_EQ(p3, p1);
    ASSERT_NE(-1, heap_check());

    // Increase by a byte and it should extend again
    p3 = REALLOC(p1, 109);
    ASSERT_EQ(128, sizeof_block(p3));
    ASSERT_PTR_EQ(p3, p1);
    ASSERT_NE(-1, heap_check());

    // This time reduce by 1.5 blocks
    p3 = REALLOC(p1, 100);
    ASSERT_EQ(104, sizeof_block(p3));
    ASSERT_PTR_EQ(p3, p1);
    ASSERT_NE(-1, heap_check());

    // Increase by a byte and it should extend by 8 bytes
    p3 = REALLOC(p1, 101);
    ASSERT_EQ(112, sizeof_block(p3));
    ASSERT_PTR_EQ(p3, p1);
    ASSERT_NE(-1, heap_check());

    //
    //p2 = realloc(p1, 50);


    FREE(p1);
    FREE(p2);

    END_TEST(stdlib_malloc);
}


int stdlib_malloc_test()
{
    CALL_TEST(stdlib_malloc, malloc1);
    CALL_TEST(stdlib_malloc, malloc2);
    CALL_TEST(stdlib_malloc, realloc);


    END_TEST_GROUP(stdlib_malloc);
}
