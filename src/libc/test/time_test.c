
#include "../time/_Time.h"

#include "test_harness.h"

#include <stdio.h>

TEST_GROUP(time)

TEST(time, tm)
{
    int expected_day_of_year;
    int expected_day_of_week;

    /* Set the testing range */
    int ystart = -1000;
    int yend = 2050;

    int did_epoch_check = 0;

    struct tm tm;
    tm.tm_hour = 0;
    tm.tm_min = 0;
    tm.tm_sec = 0;
    tm.tm_year = ystart - 1900;
    tm.tm_mon = 0;
    tm.tm_mday = 1;
    tm.tm_wday = 0;

    /* Work out the epoch of our starting point */
    time_t expected_tt = mktime(&tm);

    /* 1/1/-1000 was a Wednesday according to https://www.fourmilab.ch/documents/calendar/ */
    expected_day_of_week = 3;

    for (int y = ystart; y < yend; ++y)
    {
        expected_day_of_year = 0;
        for (int m = 0; m < 12; ++m)
        {
            int e = days_in_month(y, m);
            for (int d = 1; d <= e; ++d)
            {
                /* Convert to a time_t */
                tm.tm_year = y - 1900;
                tm.tm_mon = m;
                tm.tm_mday = d;
                time_t tt = mktime(&tm);

                /* Check the time_t is correct */
                ASSERT_EQ(expected_tt, tt);

                /* Convert back to a struct tm */
                struct tm *tm2 = gmtime(&tt);

                /* Check that the tms match */
                ASSERT_EQ(tm.tm_sec, tm2->tm_sec);
                ASSERT_EQ(tm.tm_min, tm2->tm_min);
                ASSERT_EQ(tm.tm_hour, tm2->tm_hour);
                ASSERT_EQ(tm.tm_mday, tm2->tm_mday);
                ASSERT_EQ(tm.tm_mon, tm2->tm_mon);
                ASSERT_EQ(tm.tm_year, tm2->tm_year);
                ASSERT_EQ(expected_day_of_year, tm2->tm_yday);
                ASSERT_EQ(expected_day_of_week, tm2->tm_wday);

                /* Spot check */
                if (tt >= 0 && tt < 24 * 3600)
                {
                    /* Wed Jan 1, 1970 */
                    ASSERT_EQ(1, tm.tm_mday);
                    ASSERT_EQ(0, tm.tm_mon);
                    ASSERT_EQ(70, tm.tm_year);
                    ASSERT_EQ(4, tm.tm_wday);
                    did_epoch_check = 1;
                }

                expected_tt += 3600 * 24;
                ++expected_day_of_year;
                ++expected_day_of_week;
                if (expected_day_of_week > 6)
                {
                    expected_day_of_week = 0;
                }
            }
        }
    }

    ASSERT_EQ(1, did_epoch_check);

    END_TEST(time);
}

int time_test()
{
    CALL_TEST(time, tm);

    END_TEST_GROUP(time);
}
