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
#include "common.h"

/******************************************************************************
 * The function check whether a given pixel (terminal character) is inside an
 * area, which is a two-dimensional array of blocks. The position of the area
 * is the upper left corner. The size is the number of pixels (characters) of
 * a block.
 *****************************************************************************/

bool s_area_is_inside(const s_area *area, const int row, const int col) {

	log_debug("pixel: %d/%d lb: %d/%d ub: %d/%d", row, col, area->pos.row, area->pos.col, area->pos.row + area->dim.row * area->size.row, area->pos.col + area->dim.col * area->size.col);

	//
	// Upper left corner
	//
	if (row < area->pos.row || col < area->pos.col) {
		return false;
	}

	//
	// lower right corner
	//
	if (row >= area->pos.row + area->dim.row * area->size.row || col >= area->pos.col + area->dim.col * area->size.col) {
		return false;
	}

	return true;
}

/******************************************************************************
 * The function gets the block for an absolute pixel.
 *****************************************************************************/

void s_area_get_block(const s_area *area, const s_point *pixel, s_point *block) {

	if (!s_area_is_inside(area, pixel->row, pixel->col)) {
		log_exit("pixel: %d/%d not inside", pixel->row, pixel->col);
	}

	block->row = (pixel->row - area->pos.row) / area->size.row;
	block->col = (pixel->col - area->pos.col) / area->size.col;

	log_debug("pixel - row: %d col: %d block - row: %d col: %d", pixel->row, pixel->col, block->row, block->col);
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

void s_area_print_block(const s_area *area, const int row, const int col, const wchar_t ch) {

	const int ul_row = block_upper_left(area->pos.row, area->size.row, row);
	const int ul_col = block_upper_left(area->pos.col, area->size.col, col);

	const int lr_row = ul_row + area->size.row;
	const int lr_col = ul_col + area->size.col;

	for (int r = ul_row; r < lr_row; r++) {
		for (int c = ul_col; c < lr_col; c++) {
			mvprintw(r, c, "%lc", ch);
		}
	}
}

/******************************************************************************
 * The function computes the offset, which is the relative upper left corner of
 * of the first none empty block in an area.
 *
 * Assume the area is at the home position and the mouse is not pressed. If the
 * user presses the mouse, this can be inside or outside the home area.
 *
 * If it is inside the home area, the mouse selects a pixel of the area.
 *
 * If it is outside the home area, the area is moved and the selected pixel is
 * the upper left corner of the moved area. But this is only true, if this
 * block is not empty. Otherwise it is the upper left corner of the first none
 * empty corner.
 *
 * TODO: check if it is better to compute the used area and subtract the upper
 * left corner of the used area from the area position.
 *****************************************************************************/

void s_area_get_offset(const s_area *area, s_point *offset) {

	for (int row = 0; row < area->dim.row; row++) {
		for (int col = 0; col < area->dim.col; col++) {

			//
			// We are looking for the first none empty block.
			//
			if (area->blocks[row][col] == CLR_NONE) {
				continue;
			}

			//
			// Compute the relative upper left corner and we are done.
			//
			offset->row = row * area->size.row;
			offset->col = col * area->size.col;

			return;
		}
	}
}

// ---- OK -------below -------------------------------------------------------

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
 * The function computes the used area of a block. A block may contain empty
 * rows or columns at the beginning or the end.
 *****************************************************************************/

void s_area_get_used_area(s_area *area, s_used_area *used_area) {

	//
	// Initialize the upper left (used index) with values that are to too high.
	//
	used_area->idx.row = area->dim.row;
	used_area->idx.col = area->dim.col;

	//
	// Initialize the lower left corner with values that are too small.
	//
	s_point lower_right = { -1, -1 };

	for (int row = 0; row < area->dim.row; row++) {
		for (int col = 0; col < area->dim.col; col++) {

			if (area->blocks[row][col] == CLR_NONE) {
				continue;
			}

			//
			// Decrease the upper left corner
			//
			if (used_area->idx.row > row) {
				used_area->idx.row = row;
			}

			if (used_area->idx.col > col) {
				used_area->idx.col = col;
			}

			//
			// Increase the lower right corner
			//
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
	used_area->dim.row = lower_right.row - used_area->idx.row + 1;
	used_area->dim.col = lower_right.col - used_area->idx.col + 1;

	//
	// Add the underlying area.
	//
	used_area->area = area;

	log_debug("ul: %d/%d lr: %d/%d dim: %d/%d", used_area->idx.row, used_area->idx.col, lower_right.row, lower_right.col, used_area->dim.row, used_area->dim.col);
}

/******************************************************************************
 * The function check whether the used area can be dropped anywhere on the
 * other area.
 *****************************************************************************/

bool s_area_can_drop_anywhere(s_area *area, s_used_area *used_area) {

	//
	// Compute the end index to ensure that the used area fits in the other.
	//
	const int row_end = area->dim.row - used_area->dim.row;
	const int col_end = area->dim.col - used_area->dim.col;

	s_point start;

	for (start.row = 0; start.row < row_end; start.row++) {
		for (start.col = 0; start.col < col_end; start.col++) {

			//
			// Check if the used area can be dropped at this place.
			//
			if (s_area_drop(area, &start, used_area, false)) {
				return true;
			}
		}
	}

	return false;
}

/******************************************************************************
 * The function check whether the used area is inside the game area, which is a
 * requirement for dropping the used area.
 * The function checks whether the upper left and the lower right corner of the
 * used area is inside the game area.
 *****************************************************************************/

bool s_area_used_area_is_inside(const s_area *area, const s_used_area *used_area) {

	//
	// Upper left corner
	//
	const int ul_row = used_area->area->pos.row + used_area->idx.row * area->size.row;
	const int ul_col = used_area->area->pos.col + used_area->idx.col * area->size.col;

	if (!s_area_is_inside(area, ul_row, ul_col)) {
		log_debug("used area - upper left not inside: %d/%d", ul_row, ul_col);
		return false;
	}

	//
	// Lower right corner
	//
	const int lr_row = ul_row + (used_area->dim.row - 1) * area->size.row;
	const int lr_col = ul_col + (used_area->dim.col - 1) * area->size.col;

	if (!s_area_is_inside(area, lr_row, lr_col)) {
		log_debug("used area - lower right not inside: %d/%d", lr_row, lr_col);
		return false;
	}

	log_debug("used area - is inside ul: %d/%d area:", ul_row, ul_col);

	return true;
}

/******************************************************************************
 * The function can be used to drop a used area on the game area or to check if
 * it is possible. It is assumed that the used area fits in the game area. So
 * the function checks whether there is already a block at the position, which
 * blocks the dropping.
 *****************************************************************************/

bool s_area_drop(s_area *area, const s_point *idx, s_used_area *used_area, const bool do_drop) {

	for (int row = 0; row < used_area->dim.row; row++) {
		for (int col = 0; col < used_area->dim.col; col++) {

			//
			// If the block of the used area is not defined, there is nothing
			// to do or to check.
			//
			if (used_area->area->blocks[used_area->idx.row + row][used_area->idx.col + col] == CLR_NONE) {
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
				area->blocks[idx->row + row][idx->col + col] = used_area->area->blocks[used_area->idx.row + row][used_area->idx.col + col];
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
				// REset the marked area.
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
 * The function is called after a used block is dropped. It checks if blocks of
 * the game area should disappear. If more than 4 blocks with the same color
 * are neighbors, they should be removed.
 *
 * The function returns the number of removed blocks.
 *****************************************************************************/

int s_area_remove_blocks(s_area *area, const s_point *idx, s_used_area *used_area, t_block **marks) {
	int total = 0;
	int num;

	t_block color;

	//
	// Iterate over the blocks of the used area.
	//
	for (int row = 0; row < used_area->dim.row; row++) {
		for (int col = 0; col < used_area->dim.col; col++) {

			color = used_area->area->blocks[used_area->idx.row + row][used_area->idx.col + col];

			//
			// If the current, dropped block of the area has no color, it
			// cannot be the trigger for a removing.
			//
			if (color == CLR_NONE) {
				log_debug("drop (used) empty: %d/%d", row, col);
				continue;
			}

			//
			// If the block of the used area has a color, we mark all neighbors
			// with the same color.
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
