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
#include "colors.h"

/******************************************************************************
 * The function creates a 2-dimensional array of blocks.
 *****************************************************************************/

t_block** blocks_create(const int rows, const int cols) {

	log_debug("Creating block with: %d/%d", rows, cols);

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

/******************************************************************************
 * The function initializes / sets a 2-dimensional array of blocks with a given
 * value.
 *****************************************************************************/

void blocks_set(t_block **blocks, const s_point *dim, const t_block value) {

	for (int row = 0; row < dim->row; row++) {
		for (int col = 0; col < dim->col; col++) {
			blocks[row][col] = value;
		}
	}
}

/******************************************************************************
 * The function removes all marked blocks from the game area. As a side effect
 * the marked area is reset.
 *****************************************************************************/

void blocks_remove_marked(t_block **blocks, t_block **marks, const s_point *dim) {

	for (int row = 0; row < dim->row; row++) {
		for (int col = 0; col < dim->col; col++) {

			if (marks[row][col] > 0) {
				blocks[row][col] = color_none;
				marks[row][col] = 0;
			}
		}
	}
}

/******************************************************************************
 * The function computes the used area of a block. A block may contain empty
 * rows or columns at the beginning or the end.
 *****************************************************************************/

void blocks_get_used_area(t_block **blocks, const s_point *dim, s_point *used_idx, s_point *used_dim) {

	//
	// Initialize the upper left (used index) and the lower right corners of
	// the used area with values that are to too small or too high.
	//
	s_point lower_right = { -1, -1 };

	used_idx->row = dim->row;
	used_idx->col = dim->col;

	for (int row = 0; row < dim->row; row++) {
		for (int col = 0; col < dim->col; col++) {

			if (blocks[row][col] == color_none) {
				continue;
			}

			if (used_idx->row > row) {
				used_idx->row = row;
			}

			if (used_idx->col > col) {
				used_idx->col = col;
			}

			if (lower_right.row < row) {
				lower_right.row = row;
			}

			if (lower_right.col < col) {
				lower_right.col = col;
			}
		}
	}

	//
	// Compute the dimension from the upper left (used index) and the lower
	// right corner of the used area.
	//
	used_dim->row = lower_right.row - used_idx->row + 1;
	used_dim->col = lower_right.col - used_idx->col + 1;

	log_debug("ul: %d/%d lr: %d/%d dim: %d/%d", used_idx->row, used_idx->col, lower_right.row, lower_right.col, used_dim->row, used_dim->col);
}

/******************************************************************************
 * The function can be used to check or to perform a drop at a given position.
 * It is assumed that the used area fits in the block area.
 *****************************************************************************/

bool blocks_drop(t_block **blocks, const s_point *idx, t_block **drop_blocks, const s_point *drop_idx, const s_point *drop_dim, const bool do_drop) {

	for (int row = 0; row < drop_dim->row; row++) {
		for (int col = 0; col < drop_dim->col; col++) {

			if (drop_blocks[drop_idx->row + row][drop_idx->col + col] == color_none) {
				continue;
			}

			if (blocks[idx->row + row][idx->col + col] != color_none) {
				return false;
			}

			if (do_drop) {
				blocks[idx->row + row][idx->col + col] = drop_blocks[drop_idx->row + row][drop_idx->col + col];
			}
		}
	}

	log_debug("can drop at: %d/%d", idx->row, idx->col);

	return true;
}

/******************************************************************************
 * The function check whether the used area can be dropped anywhere on the
 * other area.
 *****************************************************************************/

bool blocks_can_drop_anywhere(t_block **blocks, const s_point *dim, t_block **drop_blocks, const s_point *drop_idx, const s_point *drop_dim) {

	//
	// Compute the end index to ensure that the used area fits in the other.
	//
	const int row_end = dim->row - drop_dim->row;
	const int col_end = dim->col - drop_dim->col;

	s_point start;

	for (start.row = 0; start.row < row_end; start.row++) {
		for (start.col = 0; start.col < col_end; start.col++) {

			//
			// Check if the used area can be dropped at this place.
			//
			if (blocks_drop(blocks, &start, drop_blocks, drop_idx, drop_dim, false)) {
				return true;
			}
		}
	}

	return false;
}

/******************************************************************************
 * The function returns a struct with the total size of the area.
 *****************************************************************************/

s_point blocks_get_size(const s_point *dim, const s_point *size) {
	s_point result;

	result.row = dim->row * size->row;
	result.col = dim->col * size->col;

	log_debug("size: %d/%d", result.row, result.col);

	return result;
}
