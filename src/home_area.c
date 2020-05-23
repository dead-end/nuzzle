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

#include "home_area.h"

/******************************************************************************
 * Definitions for the home area.
 *****************************************************************************/

//
// The home position (upper left corner) of the new blocks.
//
s_point _home_pos;

//
// The dimension of the blocks of the home area
//
static s_point _home_dim;

//
// The size of a block in the home area
//
static s_point _home_size;

/******************************************************************************
 * The function initializes the home area.
 *****************************************************************************/

void home_area_init() {
	s_point_set(&_home_dim, 3, 3);
	s_point_set(&_home_size, 2, 4);
}

/******************************************************************************
 * The function tries to center the drop area on the home area. In theory both
 * have the same size. But the actual size of the drop area might be smaller if
 * it contains rows / columns which are completely empty.
 *****************************************************************************/

void home_area_center_pos(s_point *pos, const s_point *dim) {

	pos->row = _home_pos.row + (_home_dim.row - dim->row) / 2 * _home_size.row;
	pos->col = _home_pos.col + (_home_dim.col - dim->col) / 2 * _home_size.col;
}

/******************************************************************************
 * The function check if the pixel is inside the home area.
 *****************************************************************************/

bool home_area_contains(const s_point *pixel) {

	//
	// Upper left corner
	//
	if (pixel->row < _home_pos.row || pixel->col < _home_pos.col) {
		return false;
	}

	//
	// Lower right corner
	//
	if (pixel->row >= _home_pos.row + _home_size.row * _home_dim.row || pixel->col >= _home_pos.col + _home_size.col * _home_dim.col) {
		return false;
	}

	return true;
}

/******************************************************************************
 * The function prints a pixel, which is assumed to be part of the home area.
 *****************************************************************************/

void home_area_print_pixel(WINDOW *win, const s_point *pixel, const t_block da_color) {

	//
	// Compute the index of the "virtual" blocks. The home area has no blocks
	// defined.
	//
	const int idx_row = (pixel->row - _home_pos.row) / _home_size.row;
	const int idx_col = (pixel->col - _home_pos.col) / _home_size.col;

	//
	// The foreground color is the default (black) and the background color is
	// the color of the drop area (which might be CLR_NONE).
	//
	colors_chess_attr_char(win, CLR_NONE, da_color, colors_is_even(idx_row, idx_col));

	mvwprintw(win, pixel->row, pixel->col, "%lc", BLOCK_EMPTY);
}

/******************************************************************************
 * The function prints an empty home area. This is used for the initialization
 * and the resizing of the window.
 *****************************************************************************/

void home_area_print(WINDOW *win) {
	s_point pixel;

	const int end_row = _home_pos.row + _home_size.row * _home_dim.row;
	const int end_col = _home_pos.col + _home_size.col * _home_dim.col;

	//
	// Iterate over the absolute pixels of the home area.
	//
	for (pixel.row = _home_pos.row; pixel.row < end_row; pixel.row++) {
		for (pixel.col = _home_pos.col; pixel.col < end_col; pixel.col++) {

			//
			// There is no color defined for the drop area.
			//
			home_area_print_pixel(win, &pixel, CLR_NONE);
		}
	}
}
