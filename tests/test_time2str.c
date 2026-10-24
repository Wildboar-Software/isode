/* test_time2str.c - Unity tests for the UTCtime year conversions */

#include "unity.h"
#include "psap.h"

#include <string.h>
#include <time.h>

/* 2026-03-15 12:34:56 UTC */
static struct tm tm_2026(void) {
	struct tm tm;

	memset(&tm, 0, sizeof tm);
	tm.tm_year = 126;
	tm.tm_mon = 2;
	tm.tm_mday = 15;
	tm.tm_hour = 12;
	tm.tm_min = 34;
	tm.tm_sec = 56;
	return tm;
}

static void test_tm2ut_stores_the_full_year(void) {
	struct tm tm = tm_2026();
	UTCtime ut;

	tm2ut(&tm, &ut);
	TEST_ASSERT_EQUAL_INT(2026, ut.ut_year);
	TEST_ASSERT_EQUAL_INT(3, ut.ut_mon);
}

static void test_generalized_time_after_1999(void) {
	struct tm tm = tm_2026();
	UTCtime ut;

	tm2ut(&tm, &ut);
	TEST_ASSERT_EQUAL_STRING("20260315123456Z", time2str(&ut, 1));
}

static void test_utc_time_after_1999(void) {
	struct tm tm = tm_2026();
	UTCtime ut;

	tm2ut(&tm, &ut);
	TEST_ASSERT_EQUAL_STRING("260315123456Z", time2str(&ut, 0));
}

static void test_generalized_time_before_2000(void) {
	struct tm tm = tm_2026();
	UTCtime ut;

	tm.tm_year = 99;
	tm2ut(&tm, &ut);
	TEST_ASSERT_EQUAL_STRING("19990315123456Z", time2str(&ut, 1));
	TEST_ASSERT_EQUAL_STRING("990315123456Z", time2str(&ut, 0));
}

static void test_ut2tm_returns_years_since_1900(void) {
	struct tm tm = tm_2026();
	UTCtime ut;
	struct tm *back;

	tm2ut(&tm, &ut);
	back = ut2tm(&ut);
	TEST_ASSERT_EQUAL_INT(126, back->tm_year);
	TEST_ASSERT_EQUAL_INT(0, back->tm_wday);	/* a Sunday */
}

static void test_gtime_after_1999(void) {
	struct tm tm = tm_2026();

	TEST_ASSERT_EQUAL_INT64(1773578096L, gtime(&tm));
}

static void test_gtime_ut2tm_round_trip(void) {
	struct tm tm = tm_2026();
	UTCtime ut;

	tm2ut(&tm, &ut);
	TEST_ASSERT_EQUAL_INT64(1773578096L, gtime(ut2tm(&ut)));
}

static void test_str2utct_two_digit_year_window(void) {
	UTC u;

	u = str2utct("260315123456Z", 13);
	TEST_ASSERT_NOT_NULL(u);
	TEST_ASSERT_EQUAL_INT(2026, u->ut_year);
	u = str2utct("490101000000Z", 13);
	TEST_ASSERT_NOT_NULL(u);
	TEST_ASSERT_EQUAL_INT(2049, u->ut_year);
	u = str2utct("500101000000Z", 13);
	TEST_ASSERT_NOT_NULL(u);
	TEST_ASSERT_EQUAL_INT(1950, u->ut_year);
	u = str2utct("991231235959Z", 13);
	TEST_ASSERT_NOT_NULL(u);
	TEST_ASSERT_EQUAL_INT(1999, u->ut_year);
}

static void test_str2gent_round_trip(void) {
	UTC u;

	u = str2gent("20260315123456Z", 15);
	TEST_ASSERT_NOT_NULL(u);
	TEST_ASSERT_EQUAL_INT(2026, u->ut_year);
	TEST_ASSERT_EQUAL_STRING("20260315123456Z", time2str(u, 1));
	TEST_ASSERT_EQUAL_INT64(1773578096L, gtime(ut2tm(u)));
}

void run_test_time2str(void) {
	UnitySetTestFile(__FILE__);
	RUN_TEST(test_tm2ut_stores_the_full_year);
	RUN_TEST(test_generalized_time_after_1999);
	RUN_TEST(test_utc_time_after_1999);
	RUN_TEST(test_generalized_time_before_2000);
	RUN_TEST(test_ut2tm_returns_years_since_1900);
	RUN_TEST(test_gtime_after_1999);
	RUN_TEST(test_gtime_ut2tm_round_trip);
	RUN_TEST(test_str2utct_two_digit_year_window);
	RUN_TEST(test_str2gent_round_trip);
}
