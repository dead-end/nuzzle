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
 * The function check whether a given pixel (terminal character) is inside an
 * area, which is a two-dimensional array of blocks. The position of the area
 * is the upper left corner. The size is the number of pixels (characters) of
 * a block.
 *****************************************************************************/

bool is_inside_area(const s_point *pos, const s_point *dim, const s_point *size, const int row, const int col) {

	//
	// Upper left corner
	//
	if (row < pos->row || col < pos->col) {
		return false;
	}

	//
	// lower right corner
	//
	if (row >= pos->row + dim->row * size->row || col >= pos->col + dim->col * size->col) {
		return false;
	}

	return true;
}

