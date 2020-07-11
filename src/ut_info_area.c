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
#include "info_area.h"

/******************************************************************************
 * The function checks the cp_box_str() function.
 *****************************************************************************/

#define BUF_STR 11

static void test_cp_box_str() {
	wchar_t buf[BUF_STR];
	wchar_t tmp[BUF_STR];

	swprintf(tmp, BUF_STR, L"%lc 1234   %lc", U_VLINE, U_VLINE);

	cp_box_str(L"1234", buf, BUF_STR, U_VLINE);
	ut_check_wstr(buf, tmp, "cp_box_str: 1234");

	swprintf(tmp, BUF_STR, L"%lc 123456 %lc", U_VLINE, U_VLINE);

	cp_box_str(L"123456", buf, BUF_STR, U_VLINE);
	ut_check_wstr(buf, tmp, "cp_box_str: 123456");
}

/******************************************************************************
 * The function checks the cp_box_line() function.
 *****************************************************************************/

#define BUF_LINE 6

static void test_cp_box_line() {
	wchar_t buf[BUF_LINE];

	const wchar_t tmp[] = { U_ULCORNER, U_HLINE, U_HLINE, U_HLINE, U_URCORNER, U_TERM };

	cp_box_line(buf, BUF_LINE, U_ULCORNER, U_URCORNER, U_HLINE);

	ut_check_wstr(buf, tmp, "upper");
}

/******************************************************************************
 * The function is the a wrapper, that triggers the internal unit tests.
 *****************************************************************************/

void ut_info_area_exec() {

	test_cp_box_str();

	test_cp_box_line();
}
