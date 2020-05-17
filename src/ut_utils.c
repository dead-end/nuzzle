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

#include "common.h"

/******************************************************************************
 * The function checks whether an int parameter has the expected value or not.
 *****************************************************************************/

void ut_check_int(const int current, const int expected, const char *msg) {

	if (current != expected) {
		log_exit("[%s] current: %d expected: %d", msg, current, expected);
	}

	log_debug("[%s] OK current: %d", msg, current);
}

/******************************************************************************
 * The function checks whether an short parameter has the expected value or not.
 *****************************************************************************/

void ut_check_short(const short current, const short expected, const char *msg) {

	if (current != expected) {
		log_exit("[%s] current: %d expected: %d", msg, current, expected);
	}

	log_debug("[%s] OK current: %d", msg, current);
}

/******************************************************************************
 * The function checks whether a s_point parameter has the expected value or
 * not.
 *****************************************************************************/

void ut_check_s_point(const s_point *cur, const s_point *exp, const char *msg) {

	if (cur->row != exp->row || cur->col != exp->col) {
		log_exit("[%s] current: %d/%d expected: %d/%d", msg, cur->row, cur->col, exp->row, exp->col);
	}

	log_debug("[%s] OK current: %d/%d", msg, cur->row, cur->col);
}

/******************************************************************************
 * The function checks whether a bool parameter has the expected value or not.
 *****************************************************************************/

void ut_check_bool(const bool cur, const bool exp, const char *msg) {

	if (cur != exp) {
		log_exit("[%s] current: %s expected: %s", msg, bool_str(cur), bool_str(exp));
	}

	log_debug("[%s] OK current: %s", msg, bool_str(cur));
}

