
#ifndef TEST_DATA_H
#define TEST_DATA_H

#include <stdio.h>
#include <stdint.h>

struct double_test_data_s
{
    const char *str;
    uint32_t high;
    uint32_t low;
};

extern const struct double_test_data_s base_10_parser_data[];
extern const struct double_test_data_s gdtoa_test_data[];

#endif /* TEST_DATA_H */
