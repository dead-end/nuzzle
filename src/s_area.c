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

#include <ncurses.h>

#include "s_area.h"
#include "colors.h"

/******************************************************************************
 * The function copies one area to an other. The blocks are shared.
 *****************************************************************************/

void s_area_copy(s_area *to, const s_area *from) {

	to->blocks = from->blocks;

	s_point_copy(&to->dim, &from->dim);
	s_point_copy(&to->pos, &from->pos);
	s_point_copy(&to->size, &from->size);
}

/******************************************************************************
 * The function returns a struct with the total size of the area.
 *****************************************************************************/

s_point s_area_get_size(const s_area *area) {
	s_point result;

	result.row = area->dim.row * area->size.row;
	result.col = area->dim.col * area->size.col;

	log_debug("size: %d/%d", result.row, result.col);

	return result;
}

/******************************************************************************
 * The function check whether a given pixel (terminal character) is inside an
 * area, which is a two-dimensional array of blocks. The position of the area
 * is the upper left corner. The size is the number of pixels (characters) of
 * a block.
 *****************************************************************************/

bool s_area_is_inside(const s_area *area, const int row, const int col) {

	bool result = true;

	//
	// Upper left corner
	//
	if (row < area->pos.row || col < area->pos.col) {
		result = false;
	}

	//
	// 0123456789
	// 000111222
	// dim * size = 3 * 3 = 9
	//
	const int lr_row = area->pos.row + area->dim.row * area->size.row;
	const int lr_col = area->pos.col + area->dim.col * area->size.col;

	//
	// lower right corner
	//
	if (row >= lr_row || col >= lr_col) {
		result = false;
	}

	log_debug("pixel: %d/%d lb: %d/%d ub: %d/%d result: %s", row, col, area->pos.row, area->pos.col, lr_row, lr_col, boolstr(result));

	return result;
}

/******************************************************************************
 * The function gets the block for an absolute pixel.
 *****************************************************************************/

void s_area_get_block(const s_area *area, const s_point *pixel, s_point *block) {

	//
	// Ensure that the pixel is inside the area
	//
	if (!s_area_is_inside(area, pixel->row, pixel->col)) {
		log_exit("pixel: %d/%d not inside", pixel->row, pixel->col);
	}

	block->row = (pixel->row - area->pos.row) / area->size.row;
	block->col = (pixel->col - area->pos.col) / area->size.col;

	log_debug("pixel - row: %d col: %d block - row: %d col: %d", pixel->row, pixel->col, block->row, block->col);
}

/******************************************************************************
 * The function sets the blocks of the area to a given value. It is used to
 * reset the s_area.
 *****************************************************************************/

void s_area_set_blocks(const s_area *area, const t_block value) {

	for (int row = 0; row < area->dim.row; row++) {
		for (int col = 0; col < area->dim.col; col++) {
			area->blocks[row][col] = value;
		}
	}
}

/******************************************************************************
 * The function checks if the current pixel is aligned with the s_area. This
 * does not mean that the pixel is inside the s_area.
 *****************************************************************************/

bool s_area_is_aligned(const s_area *area, const int pos_row, const int pos_col) {
	return (pos_row - area->pos.row) % area->size.row == 0 && (pos_col - area->pos.col) % area->size.col == 0;
}

/******************************************************************************
 * The function checks if the position of the area is the given row and column.
 *****************************************************************************/

bool s_area_same_pos(const s_area *area, const int pos_row, const int pos_col) {

	const bool result = pos_row == area->pos.row && pos_col == area->pos.col;
	log_debug("Position %d/%d  is the same: %d", pos_row, pos_col, result);

	return result;
}

/******************************************************************************
 * The function print a block of a s_area, with a given character. It is
 * assumed that the color is already set.
 *****************************************************************************/

void s_area_print_block(WINDOW *win, const s_area *area, const int row, const int col, const wchar_t ch) {

	const int ul_row = block_upper_left(area->pos.row, area->size.row, row);
	const int ul_col = block_upper_left(area->pos.col, area->size.col, col);

	const int lr_row = ul_row + area->size.row;
	const int lr_col = ul_col + area->size.col;

	for (int r = ul_row; r < lr_row; r++) {
		for (int c = ul_col; c < lr_col; c++) {
			mvwprintw(win, r, c, "%lc", ch);
		}
	}
}

/******************************************************************************
 * The functions creates a s_area struct, by setting the values and allocating
 * the blocks.
 *****************************************************************************/

void s_area_create(s_area *area, const int dim_row, const int dim_col, const int size_row, const int size_col) {

	s_point_set(&area->dim, dim_row, dim_col);

	s_point_set(&area->size, size_row, size_col);

	area->blocks = blocks_create(area->dim.row, area->dim.col);
}

/******************************************************************************
 * The function frees the s_area struct. This is done by freeing its blocks.
 *****************************************************************************/

void s_area_free(s_area *area) {

	blocks_free(area->blocks, area->dim.row);
}

/******************************************************************************
 * The function computes the lower right corner of the used area.
 *****************************************************************************/

static void s_area_get_lr(const s_area *area, s_point *lr) {

	//
	// Initialize the lower right corner with a minimal, none valid value.
	//
	s_point_set(lr, -1, -1);

	for (int row = 0; row < area->dim.row; row++) {
		for (int col = 0; col < area->dim.col; col++) {

			if (area->blocks[row][col] == CLR_NONE) {
				continue;
			}

			//
			// Increase the corner if necessary.
			//
			if (lr->row < row) {
				lr->row = row;
			}

			if (lr->col < col) {
				lr->col = col;
			}
		}
	}

	log_debug("lower right: %d/%d", lr->row, lr->col);
}

/******************************************************************************
 * The function computes the upper left corner of the used area.
 *****************************************************************************/

static void s_area_get_ul(const s_area *area, s_point *ul) {

	//
	// Initialize the upper left corner with a maximal, none valid value.
	//
	ul->row = area->dim.row;
	ul->col = area->dim.col;

	//
	// Compute the upper left corner of the used area.
	//
	for (int row = 0; row < area->dim.row; row++) {
		for (int col = 0; col < area->dim.col; col++) {

			if (area->blocks[row][col] == CLR_NONE) {
				continue;
			}

			//
			// Decrease the upper left corner
			//
			if (ul->row > row) {
				ul->row = row;
			}

			if (ul->col > col) {
				ul->col = col;
			}
		}
	}

	log_debug("upper left: %d/%d", ul->row, ul->col);
}

/******************************************************************************
 * The function normalizes a newly created drop area. This is done by removing
 * empty rows and columns at the beginning. Example:
 *
 * 0 0 0
 * 0 1 1
 * 0 1 0
 *
 * To:
 *
 * 1 1 0
 * 1 0 0
 * 0 0 0
 *
 * ATTENTION: The dimension is updated, in this case to (2,2)
 *****************************************************************************/

void s_area_normalize(s_area *area) {

	//
	// Get the upper left corner of the used area.
	//
	s_point ul;
	s_area_get_ul(area, &ul);

	log_debug("ul: %d/%d -> 0/0", ul.row, ul.col);

	//
	// Step: 1
	//
	// At the first step, we want to move the used area to the upper left
	// corner.
	//
	if (ul.row != 0 || ul.col != 0) {

		//
		// Copy the used area to the origin: 0/0
		//
		for (int row = 0; row < area->dim.row; row++) {
			for (int col = 0; col < area->dim.col; col++) {

				//
				// Ensure that the target is part of the area
				//
				if (row + ul.row < area->dim.row && col + ul.col < area->dim.col) {
					area->blocks[row][col] = area->blocks[row + ul.row][col + ul.col];

				}
				//
				// If the index is outside fill the blocks with NONE.
				//
				else {
					area->blocks[row][col] = CLR_NONE;
				}
			}
		}
	}

	//
	// Step: 2
	//
	// At this step we want to remove the unused rows and columns at the end of
	// the used area. This is done by updating the dimension of the used area.
	// The requires that the dimension is set every time a block to drop is
	// produced.
	//
	s_point lr;
	s_area_get_lr(area, &lr);
	s_point_set(&area->dim, lr.row + 1, lr.col + 1);
	log_debug("new dim: %d/%d", area->dim.row, area->dim.col);
}

/******************************************************************************
 * The function check whether the drop area can be dropped anywhere on the
 * other area.
 *****************************************************************************/

bool s_area_can_drop_anywhere(s_area *area, s_area *drop_area) {

	//
	// Compute the end index to ensure that the drop area fits in the other.
	//
	const int row_end = area->dim.row - drop_area->dim.row;
	const int col_end = area->dim.col - drop_area->dim.col;

	log_debug("Check from 0/0 to %d,%d", row_end, col_end);

	s_point start;

	for (start.row = 0; start.row <= row_end; start.row++) {
		for (start.col = 0; start.col <= col_end; start.col++) {

			//
			// Check if the drop area can be dropped at this place.
			//
			if (s_area_drop(area, &start, drop_area, false)) {
				return true;
			}
		}
	}

	return false;
}

/******************************************************************************
 * The function checks whether the drop area is inside the given area.
 *****************************************************************************/

bool s_area_is_area_inside(const s_area *area, const s_area *drop_area) {

	//
	// Upper left corner
	//
	if (!s_area_is_inside(area, drop_area->pos.row, drop_area->pos.col)) {
		return false;
	}

	//
	// lower right corner index
	//
	const int lr_row = drop_area->pos.row + (drop_area->dim.row - 1) * drop_area->size.row;
	const int lr_col = drop_area->pos.col + (drop_area->dim.col - 1) * drop_area->size.col;

	log_debug("Drop: %d/%d %d/%d", drop_area->pos.row, drop_area->pos.col, lr_row, lr_col);

	if (!s_area_is_inside(area, lr_row, lr_col)) {
		return false;
	}

	return true;
}

/******************************************************************************
 * The function can be used to drop an area on the game area or to check if it
 * is possible. It is assumed that the drop area fits in the game area. So the
 * function checks whether there is already a block at the position, which
 * blocks the dropping.
 *****************************************************************************/

bool s_area_drop(s_area *area, const s_point *idx, const s_area *drop_area, const bool do_drop) {

	for (int row = 0; row < drop_area->dim.row; row++) {
		for (int col = 0; col < drop_area->dim.col; col++) {

			//
			// If the block of the used area is not defined, there is nothing
			// to do or to check.
			//
			if (drop_area->blocks[row][col] == CLR_NONE) {
				continue;
			}

			//
			// If there is a block on the game area defined, dropping is not
			// possible.
			//
			if (area->blocks[idx->row + row][idx->col + col] != CLR_NONE) {
				return false;
			}

			//
			// At this point dropping of current block is possible. The flag
			// defines whether the dropping should be performed.
			//
			if (do_drop) {
				area->blocks[idx->row + row][idx->col + col] = drop_area->blocks[row][col];
			}
		}
	}

	log_debug("can drop at: %d/%d", idx->row, idx->col);

	return true;
}

/******************************************************************************
 * The function removes all marked blocks from the game area. As a side effect
 * the marked area is reset.
 *****************************************************************************/

static void s_area_remove_marked(s_area *area, t_block **marks) {

	//
	// The game area and the marked area have the same dimensions, so we can
	// iterate over both.
	//
	for (int row = 0; row < area->dim.row; row++) {
		for (int col = 0; col < area->dim.col; col++) {

			if (marks[row][col] > 0) {

				//
				// Remove the block from the game area.
				//
				area->blocks[row][col] = CLR_NONE;

				//
				// Reset the marked area.
				//
				marks[row][col] = 0;
			}
		}
	}
}

/******************************************************************************
 * The function is called with a color and recursively marks all neighbors,
 * that have the same color. The recursion stops if all neighbors have
 * different colors or are already marked.
 *****************************************************************************/

void s_area_mark_neighbors(const s_area *area, t_block **marks, const int row, const int col, t_block color, int *num) {

	//
	// Ensure that we are on the game area. The function is called on the
	// neighbors of a block by adding and subtracting 1 to the row / column.
	//
	if (row < 0 || row >= area->dim.row || col < 0 || col >= area->dim.row) {
		log_debug("Outside: %d/%d num: %d color: %d", row, col, *num, color);
		return;
	}

	//
	// Current block has the wrong color.
	//
	if (area->blocks[row][col] != color) {
		log_debug("Wrong color: %d/%d num: %d color: %d", row, col, *num, color);
		return;
	}

	//
	// Current block is already marked.
	//
	if (marks[row][col] != 0) {
		log_debug("Already marked: %d/%d num: %d color: %d", row, col, *num, color);
		return;
	}

	//
	// Increase the number and mark the block.
	//
	marks[row][col] = ++(*num);

	log_debug("Mark: %d/%d num: %d color: %d", row, col, *num, color);

	//
	// Recursively process the neighbors.
	//
	s_area_mark_neighbors(area, marks, row + 1, col, color, num);
	s_area_mark_neighbors(area, marks, row - 1, col, color, num);
	s_area_mark_neighbors(area, marks, row, col + 1, color, num);
	s_area_mark_neighbors(area, marks, row, col - 1, color, num);
}

/******************************************************************************
 * The function is called after an area is dropped. It checks if blocks of the
 * game area should disappear. If more than 4 blocks with the same color are
 * neighbors, they should be removed.
 *
 * The function returns the number of removed blocks.
 *****************************************************************************/

int s_area_remove_blocks(s_area *area, const s_point *idx, const s_area *drop_area, t_block **marks) {
	int total = 0;
	int num;

	t_block color;

	//
	// Iterate over the blocks of the drop area.
	//
	for (int row = 0; row < drop_area->dim.row; row++) {
		for (int col = 0; col < drop_area->dim.col; col++) {

			color = drop_area->blocks[row][col];

			//
			// If the current, dropped block of the area has no color, it
			// cannot be the trigger for a removing.
			//
			if (color == CLR_NONE) {
				log_debug("drop (used) empty: %d/%d", row, col);
				continue;
			}

			//
			// If the block of the dropped area has a color, we mark all
			// neighbors with the same color.
			//
			num = 0;
			s_area_mark_neighbors(area, marks, idx->row + row, idx->col + col, color, &num);
			log_debug("num: %d", num);

			if (num < 4) {

				//
				// If the number of neighbors with the same color is less than
				// 4, we remove the marks of the marked area.
				//
				blocks_set(marks, &area->dim, 0);

			} else {

				//
				// If the number of neighbors with the same color is more than
				// 4, we have to remove the marks from the game area and reset
				// the marks area.
				//
				s_area_remove_marked(area, marks);
				total += num;
			}
		}
	}

	return total;
}
