
#include "debug.h"

void __test_harness_print_string(const char *str)
{
	while (*str)
	{
		debug_putc(*str);
		if (*str == '\n')
		{
			debug_putc('\r');
		}
		++str;
	}
}

void __test_harness_print_num(long long num)
{
	if (num == 0)
	{
		__test_harness_print_string("0");
	}

	char buf[100];
	char *p = &buf[100];
	*--p = 0;
	while (num)
	{
		int dig = num % 10;
		char c = dig + '0';
		*--p = c;
		num = num / 10;
	}
	__test_harness_print_string(p);
}

void __test_harness_print_error(const char *file, int line, const char *actual_str, long long actual, long long expected)
{
	__test_harness_print_string(file);
	__test_harness_print_string("(");
	__test_harness_print_num(line);
	__test_harness_print_string(") ");
	__test_harness_print_string(" error: value of ");
	__test_harness_print_string(actual_str);
	__test_harness_print_string("\n");
	__test_harness_print_string("  Actual: ");
	__test_harness_print_num(actual);
	__test_harness_print_string("\n");
	__test_harness_print_string("Expected: ");
	__test_harness_print_num(expected);
	__test_harness_print_string("\n");
}

void __test_harness_print_group_summary(const char *group, int pass_cnt, int fail_cnt)
{
	__test_harness_print_string("[==========] ");
	__test_harness_print_num(pass_cnt + fail_cnt);
	__test_harness_print_string(" tests from group ");
	__test_harness_print_string(group);
	__test_harness_print_string("\n");
	__test_harness_print_string("[  PASSED  ] ");
	__test_harness_print_num(pass_cnt);
	__test_harness_print_string("\n");
	__test_harness_print_string("[  FAILED  ] ");
	__test_harness_print_num(fail_cnt);
	__test_harness_print_string("\n[==========]\n\n");
}
