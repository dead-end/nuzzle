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

	cp_pad(L"1234", buf, 6, U_EMPTY);

	//
	// Check the two strings
	//
	ut_check_wstr(buf, L"1234 ", "cp_pad: - first 1234");
	ut_check_wstr(&buf[6], L"####", "cp_pad: - second ####");
}

/******************************************************************************
 * The function is the a wrapper, that triggers the internal unit tests.
 *****************************************************************************/

void ut_common_exec() {

	test_strs_dim();

	test_trim_r();

	test_cpy_str_centered();

	test_cp_pad();
}
