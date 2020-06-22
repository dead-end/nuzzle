/*
 * MIT License
 *
 * Copyright (c) 2019 dead-end
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

#include <ctype.h>
#include <errno.h>

/******************************************************************************
 * The function allocates memory and terminates the program in case of an
 * error.
 *****************************************************************************/

void* xmalloc(const size_t size) {

	void *ptr = malloc(size);

	if (ptr == NULL) {
		log_exit("Unable to allocate: %zu bytes of memory!", size);
	}

	return ptr;
}

/******************************************************************************
 * The function is called with an array of strings, which is NULL terminated.
 * It computes the maximal string length and the number of rows.
 *****************************************************************************/

s_point strs_dim(const char *strs[]) {
	s_point dim = { .row = 0, .col = -1 };

	int col;

	for (int i = 0; strs[i] != NULL; i++) {
		col = strlen(strs[i]);
		if (col > dim.col) {
			dim.col = col;
		}
		dim.row++;
	}

	return dim;
}

/******************************************************************************
 * The function removes the tailing white spaces from a string.
 *****************************************************************************/

void trim_r(char *str) {

	const size_t len = strlen(str);

	for (int i = len - 1; i >= 0 && isspace(str[i]); i--) {
		str[i] = '\0';
	}
}

/******************************************************************************
 * The function converts a sting to an integer value.
 *****************************************************************************/

int str_2_int(const char *str) {
	char *tmp;

	errno = 0;

	const int result = (int) strtol(str, &tmp, 10);

	//
	// Check for overflows.
	//
	if (errno != 0) {
		log_exit("Unable to convert: %s - %s", str, strerror(errno));
	}

	//
	// Ensure that the whole value was converted.
	//
	if (*tmp != '\0') {
		log_exit("Unable to convert: %s - %s", str, tmp);
	}

	return result;
}

/******************************************************************************
 * The function copies the source string centered to the target string.
 *
 * Example:
 *
 * - source string: 123
 * - target size: 9 (with \0)
 *
 * 012345678
 *   1234  \0
 *
 * (Unit tested)
 *****************************************************************************/

void cpy_str_centered(char *to, const int size, const char *from) {

	const int src_len = strlen(from);
	const int start = center(size - 1, src_len);
	const int end = start + src_len;

	//
	// Copy the source to the destination with the padding.
	//
	for (int i = 0; i < size - 1; i++) {

		if (i < start || i >= end) {
			to[i] = ' ';

		} else {
			to[i] = from[i - start];
		}
	}

	//
	// Add terminating \0 to the target
	//
	to[size - 1] = '\0';

	log_debug("source: '%s' target: '%s' start-idx: %d end-idx: %d source-len: %d size: %d", from, to, start, end, src_len, size);
}
