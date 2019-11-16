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
 * The function check whether a given pixel (terminal character) is inside an
 * area, which is a two-dimensional array of blocks. The position of the area
 * is the upper left corner. The size is the number of pixels (characters) of
 * a block.
 *****************************************************************************/

bool s_area_is_inside(const s_area *area, const int row, const int col) {

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
 * The function is used, in case the user presses the mouse button outside the
 * home area of the new area.
 *****************************************************************************/

void s_area_get_offset(const s_area *area, s_point *offset) {

	for (int row = 0; row < area->dim.row; row++) {
		for (int col = 0; col < area->dim.col; col++) {

			//
			// We are looking for the first none empty block.
			//
			if (area->blocks[row][col] == color_none) {
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
