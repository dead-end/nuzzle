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
 * The function is the a wrapper, that triggers the internal unit tests.
 *****************************************************************************/

void ut_common_exec() {

	test_cpy_str_centered();
}
