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

#include <blocks.h>

#include "common.h"

/******************************************************************************
 * The function initializes a 2-dimensional array of blocks with a given value.
 *****************************************************************************/

void blocks_init(t_block **blocks, const int rows, const int cols, const t_block value) {

	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {
			blocks[row][col] = value;
		}
	}
}

/******************************************************************************
 * The function creates a 2-dimensional array of blocks.
 *****************************************************************************/

t_block** blocks_create(const int rows, const int cols) {

	t_block **blocks = xmalloc(sizeof(t_block*) * rows);

	for (int row = 0; row < rows; row++) {
		blocks[row] = xmalloc(sizeof(t_block) * cols);
	}

	return blocks;
}

/******************************************************************************
 * The function frees the allocated data.
 *****************************************************************************/

void blocks_free(t_block **blocks, const int rows) {

	for (int row = 0; row < rows; row++) {
		free(blocks[row]);
	}

	free(blocks);
}
