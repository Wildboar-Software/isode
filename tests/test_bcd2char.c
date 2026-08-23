/* test_bcd2char.c - Unity tests for bcd2char */

#include "unity.h"
#include "general.h"

#include <stdint.h>
#include <string.h>

static void test_bcd2char_even_decimal_digits(void) {
	uint8_t src[] = { 0x12, 0x34 };
	char dst[8];
	int n;

	memset(dst, 0x7f, sizeof dst);
	n = bcd2char(src, dst, 4);
	TEST_ASSERT_EQUAL_INT(4, n);
	TEST_ASSERT_EQUAL_STRING("1234", dst);
}

static void test_bcd2char_odd_length_uses_high_nibble_only(void) {
	uint8_t src[] = { 0x12, 0x3f };
	char dst[8];
	int n;

	n = bcd2char(src, dst, 3);
	TEST_ASSERT_EQUAL_INT(3, n);
	TEST_ASSERT_EQUAL_STRING("123", dst);
}

static void test_bcd2char_hex_letters_are_lowercase(void) {
	uint8_t src[] = { 0xab, 0xcd, 0xef };
	char dst[8];
	int n;

	n = bcd2char(src, dst, 6);
	TEST_ASSERT_EQUAL_INT(6, n);
	TEST_ASSERT_EQUAL_STRING("abcdef", dst);
}

static void test_bcd2char_zero_length(void) {
	uint8_t src[] = { 0xff };
	char dst[8];
	int n;

	memset(dst, 0x7f, sizeof dst);
	n = bcd2char(src, dst, 0);
	TEST_ASSERT_EQUAL_INT(0, n);
	TEST_ASSERT_EQUAL_STRING("", dst);
}

static void test_bcd2char_single_nibble(void) {
	uint8_t src[] = { 0x50 };
	char dst[8];
	int n;

	n = bcd2char(src, dst, 1);
	TEST_ASSERT_EQUAL_INT(1, n);
	TEST_ASSERT_EQUAL_STRING("5", dst);
}

void run_test_bcd2char(void);
void run_test_bcd2char(void) {
	UnitySetTestFile(__FILE__);
	RUN_TEST(test_bcd2char_even_decimal_digits);
	RUN_TEST(test_bcd2char_odd_length_uses_high_nibble_only);
	RUN_TEST(test_bcd2char_hex_letters_are_lowercase);
	RUN_TEST(test_bcd2char_zero_length);
	RUN_TEST(test_bcd2char_single_nibble);
}
