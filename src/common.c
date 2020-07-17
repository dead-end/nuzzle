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
#include <wchar.h>
#include <stdarg.h>

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
 *
 * (Unit tested)
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
 *
 * (Unit tested)
 *****************************************************************************/

void trim_r(char *str) {

	const size_t len = strlen(str);

	for (int i = len - 1; i >= 0 && isspace(str[i]); i--) {
		str[i] = '\0';
	}
}

/******************************************************************************
 * The function converts a sting to an integer value.
 *
 * The function is not unit tested. The interesting part is the error handling.
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

char* cpy_str_centered(char *to, const int size, const char *from) {

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

	return to;
}

/******************************************************************************
 * The function copies the source wchar_t string to the destination. If the
 * source string is smaller than the destination, the destination is filled
 * with padding chars. The terminating \0 is always set.
 *
 * The size parameter contains the size of the buffer including the \0.
 *
 * (Unit tested)
 *****************************************************************************/

void cp_pad(const wchar_t *src, wchar_t *dst, const int size, const wchar_t pad) {

	const int len = wcslen(src);

	if (len > size - 1) {
		log_exit("String to long: %ls", src);
	}

	wmempcpy(dst, src, len);

	//
	// If the source is smaller we do the padding.
	//
	if (len < size - 1) {
		wmemset(&dst[len], pad, size - 1 - len);
	}

	//
	// Set the terminating \0.
	//
	dst[size - 1] = U_TERM;
}

/******************************************************************************
 * The function creates a a formated string with a given size. If the result of
 * formating is smaller than the size, the string will be padded with the
 * padding character. Example:
 *
 * fmt_pad(dst, 11, L'#', L"12%d", 34);
 *
 * 01234567890  <- index
 * 1234######\0 <- result
 *
 * The size parameter contains the size of the buffer including the \0.
 *
 * (Unit tested)
 *****************************************************************************/

void fmt_pad(wchar_t *dst, const int size, const wchar_t pad, const wchar_t *fmt, ...) {

	//
	// Start variable argument lists
	//
	va_list argp;
	va_start(argp, fmt);

	const int result = vswprintf(dst, size, fmt, argp);

	//
	// Error checking.
	//
	if (result == -1) {
		log_exit("Calling: vswprintf() failed: %ls", fmt);
	}

	//
	// The result was truncated.
	//
	if (result >= size) {
		log_exit("Truncated: %ls", dst);
	}

	va_end(argp);

	//
	// If the result fits the size, we do not need a padding and so we are
	// done.
	//
	if (result == size - 1) {
		log_debug_str("Fitting");
		return;
	}

	//
	// Do the padding.
	//
	wmemset(&dst[result], pad, size - 1 - result);

	//
	// Set the terminating \0.
	//
	dst[size - 1] = U_TERM;
}

/******************************************************************************
 * The function creates a a formated string with a given size. If the result of
 * formating is smaller than the size, the string will centered. Example
 *
 * fmt_center(dst, 11, L'#', L"12%d", 34);
 *
 * 01234567890  <- index
 * ###1234###\0 <- result
 *
 * The size parameter contains the size of the buffer including the \0.
 *
 * (Unit tested)
 *****************************************************************************/

void fmt_center(wchar_t *dst, const int size, const wchar_t pad, const wchar_t *fmt, ...) {
	wchar_t tmp[size];

	//
	// Start variable argument lists
	//
	va_list argp;
	va_start(argp, fmt);

	const int result = vswprintf(tmp, size, fmt, argp);

	//
	// Error checking.
	//
	if (result == -1) {
		log_exit("Calling: vswprintf() failed: %ls", fmt);
	}

	//
	// The result was truncated.
	//
	if (result >= size) {
		log_exit("Truncated: %ls", tmp);
	}

	va_end(argp);

	//
	// Fill the buffer with the padding character. The is only necessary if
	// there is space left for padding.
	//
	if (size - 1 > result) {
		wmemset(dst, pad, size - 1);
	}

	//
	// Compute the starting index.
	//
	const int start = center(size - 1, result);

	//
	// Copy the result
	//
	wmemcpy(&dst[start], tmp, result);

	//
	// Set the terminating \0.
	//
	dst[size - 1] = U_TERM;
}
