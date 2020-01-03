
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../stdio/_Anvil_stdio.h"

#include "test_harness.h"

#pragma GCC diagnostic ignored "-Woverflow"

TEST_GROUP(stdio_io)

char *test_buffer;

long _Anvil_write(int fildes, const void *buf, size_t nbyte)
{
    strncat(test_buffer, buf, nbyte);
    //printf("Writing <%.*s>\n", nbyte, buf);
    return nbyte;
}

int open_flags;

int _Anvil_open(const char *path, int oflag)
{
    open_flags = oflag;
    return 3;
}

int _Anvil_close(int fd)
{
    return 0;
}

TEST(stdio_io, fopen)
{
    static struct
    {
        const char *flag_str;
        int open_flags;
        int stream_flags;
    } test_data[] =
    {
        { "r",   O_RDONLY,                              _ANVIL_STDIO_READABLE },
        { "w",   O_WRONLY | O_CREAT | O_TRUNC,          _ANVIL_STDIO_WRITEABLE },
        { "wx",  O_WRONLY | O_CREAT | O_TRUNC | O_EXCL, _ANVIL_STDIO_WRITEABLE },
        { "a",   O_WRONLY | O_CREAT | O_APPEND,         _ANVIL_STDIO_WRITEABLE },
        { "r+",  O_RDWR,                                _ANVIL_STDIO_READABLE | _ANVIL_STDIO_WRITEABLE },
        { "w+",  O_RDWR | O_CREAT | O_TRUNC,            _ANVIL_STDIO_READABLE | _ANVIL_STDIO_WRITEABLE },
        { "w+x", O_RDWR | O_CREAT | O_TRUNC | O_EXCL,   _ANVIL_STDIO_READABLE | _ANVIL_STDIO_WRITEABLE },
        { "a+",  O_RDWR | O_CREAT | O_APPEND,           _ANVIL_STDIO_READABLE | _ANVIL_STDIO_WRITEABLE },
    };

    FILE *f;
    size_t i;

    for (i=0; i< sizeof(test_data) / sizeof(test_data[0]); ++i)
    {
        open_flags = 0;
        f = fopen("abc.txt", test_data[i].flag_str);
        ASSERT_PTR_NE(NULL, f);
        ASSERT_EQ(test_data[i].open_flags, open_flags);
        ASSERT_EQ(test_data[i].stream_flags, f->__status & (_ANVIL_STDIO_READABLE|_ANVIL_STDIO_WRITEABLE));
        ASSERT_EQ(0, fclose(f));
    }

    END_TEST(stdio_io);
}

TEST(stdio_io, _Anvil_fputc)
{
    test_buffer = calloc(1, 600);

    // Put 510 A's in the buffer
    for (int i=0; i<510; ++i)
    {
        ASSERT_EQ('A', _Anvil_fputc('A', stdout));
        ASSERT_EQ(0, strlen(test_buffer));
    }

    // Add a B
    ASSERT_EQ('B', _Anvil_fputc('B', stdout));
    ASSERT_EQ(0, strlen(test_buffer));

    // Add a C to cause a flush
    ASSERT_EQ('C', _Anvil_fputc('C', stdout));
    ASSERT_EQ(512, strlen(test_buffer));

    // Now add 10 more chars and flush manually
    for (int i=0; i<10; ++i)
    {
        ASSERT_EQ('D', _Anvil_fputc('D', stdout));
        ASSERT_EQ(512, strlen(test_buffer));
    }
    _Anvil_fflush(stdout);
    ASSERT_EQ(522, strlen(test_buffer));

    // Now add 1 more char and flush manually
    ASSERT_EQ('E', _Anvil_fputc('E', stdout));
    ASSERT_EQ(522, strlen(test_buffer));
    _Anvil_fflush(stdout);
    ASSERT_EQ(523, strlen(test_buffer));

    // Check the entire buffer contents
    for (int i=0; i<510; ++i)
    {
        ASSERT_EQ('A', test_buffer[i]);
    }
    ASSERT_EQ('B', test_buffer[510]);
    ASSERT_EQ('C', test_buffer[511]);
    for (int i=512; i<522; ++i)
    {
        ASSERT_EQ('D', test_buffer[i]);
    }
    ASSERT_EQ('E', test_buffer[522]);

    free(test_buffer);

    END_TEST(stdio_io);
}

int stdio_io_test()
{
    CALL_TEST(stdio_io, fopen);
    CALL_TEST(stdio_io, _Anvil_fputc);

    END_TEST_GROUP(stdio_io);
}
