/*
 * MIT License
 *
 * Copyright (c) 2020 dead-end
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ut_utils.h"

/******************************************************************************
 * The function checks the strs_dim() function.
 *****************************************************************************/

static void test_strs_dim() {
	const char *str[] = { "1", "12", "123", "1234", "1", NULL };

	const s_point dim = strs_dim(str);

	ut_check_s_point(&dim, &(s_point ) { 5, 4 }, "test_strs_dim");
}

/******************************************************************************
 * The function checks the trim_r() function.
 *****************************************************************************/

static void test_trim_r() {
	char str[] = "012  ";

	trim_r(str);

	ut_check_str(str, "012", "test_trim_r");
}

/******************************************************************************
 * The function checks the cpy_str_centered() function, which copies the source
 * string centered to the destination string.
 *****************************************************************************/

#define BUF_SIZE_1 9
#define BUF_SIZE_2 10

static void test_cpy_str_centered() {
	const char *from_1 = "1234";
	const char *from_2 = "123";

	char to_1[BUF_SIZE_1];
	char to_2[BUF_SIZE_2];

	//
	// strlen(dst) - strlen(src) odd
	//
	cpy_str_centered(to_1, BUF_SIZE_1, from_1);
	ut_check_str(to_1, "  1234  ", "check center 1");

	//
	// strlen(dst) - strlen(src) even
	//
	cpy_str_centered(to_2, BUF_SIZE_2, from_1);
	ut_check_str(to_2, "  1234   ", "check center 2");

	//
	// strlen(dst) - strlen(src) even
	//
	cpy_str_centered(to_1, BUF_SIZE_1, from_2);
	ut_check_str(to_1, "  123   ", "check center 3");

	//
	// strlen(dst) - strlen(src) odd
	//
	cpy_str_centered(to_2, BUF_SIZE_2, from_2);
	ut_check_str(to_2, "   123   ", "check center 4");
}

/******************************************************************************
 * The function checks the cp_pad() function.
 *****************************************************************************/

#define BUF_STR 11

#define BUF_FIRST 6

static void test_cp_pad() {
	wchar_t buf[BUF_STR];

	//
	// Fill the destination string with data, to see that it works.
	//
	wmemset(buf, L'#', BUF_STR - 1);
	buf[BUF_STR - 1] = U_TERM;

	//
	// Source string is smaller, so a padding is expected.
	//
	cp_pad(L"1234", buf, BUF_STR, U_EMPTY);
	ut_check_wstr(buf, L"1234      ", "cp_pad: 1234");

	//
	// Source string has the same length, so it is a simple copy.
	//
	cp_pad(L"1234567890", buf, BUF_STR, U_EMPTY);
	ut_check_wstr(buf, L"1234567890", "cp_pad: 1234567890");

	//
	// We what to ensure that the padding does not write over the limits.
	//
	// 01234567890 <- index
	// ##########T <- init with T as \0
	// 1234 T####T <- result
	//
	wmemset(buf, L'#', BUF_STR - 1);
	buf[BUF_STR - 1] = U_TERM;

	cp_pad(L"1234", buf, BUF_FIRST, U_EMPTY);

	//
	// Check the two strings
	//
	ut_check_wstr(buf, L"1234 ", "cp_pad: - first 1234");
	ut_check_wstr(&buf[BUF_FIRST], L"####", "cp_pad: - second ####");
}

/******************************************************************************
 * Simple function that fills a wchar_t with a character.
 *****************************************************************************/

static void buf_fill(wchar_t *buf, const int size, const wchar_t wch) {
	wmemset(buf, wch, size - 1);
	buf[size - 1] = U_TERM;
}

/******************************************************************************
 * The function checks the fmt_pad() function. We are filling the destination
 * string with '#' to see, that all is overwritten.
 *****************************************************************************/

static void test_fmt_pad() {
	wchar_t buf[BUF_STR];

	//
	// Source string is smaller, so a padding is expected.
	//
	buf_fill(buf, BUF_STR, L'#');
	fmt_pad(buf, BUF_STR, U_EMPTY, L"12%d", 34);

	ut_check_wstr(buf, L"1234      ", "cp_pad - short");

	//
	// Source string has the same length, so it is a simple copy.
	//
	buf_fill(buf, BUF_STR, L'#');
	fmt_pad(buf, BUF_STR, U_EMPTY, L"123456%d", 7890);

	ut_check_wstr(buf, L"1234567890", "cp_pad - same");

	//
	// We what to ensure that the padding does not write over the limits. We
	// are using only half of the buffer. The rest should be untouched.
	//
	// 01234567890 <- index
	// ##########T <- init with T as \0
	// 1234 T####T <- result
	//
	buf_fill(buf, BUF_STR, L'#');
	fmt_pad(buf, BUF_FIRST, U_EMPTY, L"12%d", 34);

	//
	// Check the two strings
	//
	ut_check_wstr(buf, L"1234 ", "cp_pad - first sort");
	ut_check_wstr(&buf[BUF_FIRST], L"####", "cp_pad - second short");

	//
	// Second test where the source string has the same length.
	//
	// 01234567890 <- index
	// ##########T <- init with T as \0
	// 12345T####T <- result
	//
	buf_fill(buf, BUF_STR, L'#');
	fmt_pad(buf, BUF_FIRST, U_EMPTY, L"12%d", 345);

	//
	// Check the two strings
	//
	ut_check_wstr(buf, L"12345", "cp_pad - first same");
	ut_check_wstr(&buf[BUF_FIRST], L"####", "cp_pad - second same");
}

/******************************************************************************
 * The function checks the fmt_center() function.
 *****************************************************************************/

static void test_fmt_center() {
	wchar_t buf[BUF_STR];

	//
	// "01234567890"
	// "   1234   "
	//
	buf_fill(buf, BUF_STR, L'#');
	fmt_center(buf, BUF_STR, U_EMPTY, L"12%d", 34);

	ut_check_wstr(buf, L"   1234   ", "cp_center - short");

	//
	// Source string has the same length, so it is a simple copy.
	//
	buf_fill(buf, BUF_STR, L'#');
	fmt_center(buf, BUF_STR, U_EMPTY, L"123456%d", 7890);

	ut_check_wstr(buf, L"1234567890", "cp_center - same");

	//
	// We what to ensure that the padding does not write over the limits. We
	// are using only half of the buffer. The rest should be untouched.
	//
	// 01234567890 <- index
	// ##########T <- init with T as \0
	//  123 T####T <- result
	//
	buf_fill(buf, BUF_STR, L'#');
	fmt_center(buf, BUF_FIRST, U_EMPTY, L"12%d", 3);

	//
	// Check the two strings
	//
	ut_check_wstr(buf, L" 123 ", "cp_center - first short");
	ut_check_wstr(&buf[BUF_FIRST], L"####", "cp_center - second short");

	//
	// Second test where the source string has the same length.
	//
	// 01234567890 <- index
	// ##########T <- init with T as \0
	// 12345T####T <- result
	//
	buf_fill(buf, BUF_STR, L'#');
	fmt_center(buf, BUF_FIRST, U_EMPTY, L"12%d", 345);

	//
	// Check the two strings
	//
	ut_check_wstr(buf, L"12345", "cp_center - first same");
	ut_check_wstr(&buf[BUF_FIRST], L"####", "cp_center - second same");
}

/******************************************************************************
 * The function is the a wrapper, that triggers the internal unit tests.
 *****************************************************************************/

void ut_common_exec() {

	test_strs_dim();

	test_trim_r();

	test_cpy_str_centered();

	test_cp_pad();

	test_fmt_pad();

	test_fmt_center();
}
