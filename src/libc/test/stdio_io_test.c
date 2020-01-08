
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../stdio/_Anvil_stdio.h"

#include "test_harness.h"

#pragma GCC diagnostic ignored "-Woverflow"

TEST_GROUP(stdio_io)

char *test_buffer;
size_t read_file_offs;

long _Anvil_write(int fildes, const void *buf, size_t nbyte)
{
    strncat(test_buffer, buf, nbyte);
    //printf("Writing <%.*s>\n", nbyte, buf);
    return nbyte;
}

static const char read_file_data[500 + 11 + 7] =
    "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
    "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111"
    "22222222222"
    "3456789";

long _Anvil_read(int fildes, const void *buf, size_t nbyte)
{
    if (fildes == 4)
    {
        size_t bytes_to_read = nbyte;
        if (bytes_to_read > sizeof(read_file_data) - read_file_offs)
        {
            bytes_to_read = sizeof(read_file_data) - read_file_offs;
        }
        //printf("Reading %u %u %u bytes\n", bytes_to_read, sizeof(read_file_data), read_file_offs);
        //printf("Reading %d bytes\n", bytes_to_read);
        memcpy(buf, read_file_data + read_file_offs, bytes_to_read);
        read_file_offs += bytes_to_read;
        return bytes_to_read;
    }
    return -1;
}

int open_flags;

int _Anvil_open(const char *path, int oflag)
{
    if (!strcmp(path, "open_file.txt"))
    {
        open_flags = oflag;
        return 3;
    }
    else if (!strcmp(path, "read_file.txt"))
    {
        read_file_offs = 0;
        return 4;
    }
    else
    {
        return -1;
    }
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
        f = fopen("open_file.txt", test_data[i].flag_str);
        ASSERT_PTR_NE(NULL, f);
        ASSERT_EQ(test_data[i].open_flags, open_flags);
        ASSERT_EQ(test_data[i].stream_flags, f->__status & (_ANVIL_STDIO_READABLE|_ANVIL_STDIO_WRITEABLE));
        ASSERT_EQ(0, fclose(f));
    }

    END_TEST(stdio_io);
}

TEST(stdio_io, _Anvil_fgetc)
{
    FILE *f;

    f = fopen("read_file.txt", "r");
    ASSERT_PTR_NE(NULL, f);

    for (int i=0; i<500; ++i)
    {
        ASSERT_EQ('1', _Anvil_fgetc(f));
    }
    for (int i=0; i<11; ++i)
    {
        ASSERT_EQ('2', _Anvil_fgetc(f));
    }
    for (int i=0; i<7; ++i)
    {
        ASSERT_EQ('3' + i, _Anvil_fgetc(f));
    }
    ASSERT_EQ(EOF, _Anvil_fgetc(f));

    ASSERT_EQ(0, fclose(f));

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
    CALL_TEST(stdio_io, _Anvil_fgetc);
    CALL_TEST(stdio_io, _Anvil_fputc);

    END_TEST_GROUP(stdio_io);
}
