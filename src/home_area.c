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

//
// The home position (upper left corner) of the new blocks.
//
s_point _home;

#define DIM_ROW 3
#define DIM_COL 3

#define SIZE_ROW 2
#define SIZE_COL 4

/******************************************************************************
 * The function tries to center the drop area on the home area. In theory both
 * have the same size. But the actual size of the drop area might be smaller if
 * it contains rows / columns which are completely empty.
 *****************************************************************************/

void home_area_center_pos(s_point *pos, const s_point *dim) {

	pos->row = _home.row + (DIM_ROW - dim->row) / 2 * SIZE_ROW;
	pos->col = _home.col + (DIM_COL - dim->col) / 2 * SIZE_COL;
}

/******************************************************************************
 * The function check if the pixel is inside the home area.
 *****************************************************************************/

bool home_area_contains(const s_point *pixel) {

	//
	// Upper left corner
	//
	if (pixel->row < _home.row || pixel->col < _home.col) {
		return false;
	}

	//
	// Lower right corner
	//
	if (pixel->row >= _home.row + SIZE_ROW * DIM_ROW || pixel->col >= _home.col + SIZE_COL * DIM_COL) {
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
	const int idx_row = (pixel->row - _home.row) / SIZE_ROW;
	const int idx_col = (pixel->col - _home.col) / SIZE_COL;

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

	const int end_row = _home.row + SIZE_ROW * DIM_ROW;
	const int end_col = _home.col + SIZE_COL * DIM_COL;

	//
	// Iterate over the absolute pixels of the home area.
	//
	for (pixel.row = _home.row; pixel.row < end_row; pixel.row++) {
		for (pixel.col = _home.col; pixel.col < end_col; pixel.col++) {

			//
			// There is no color defined for the drop area.
			//
			home_area_print_pixel(win, &pixel, CLR_NONE);
		}
	}
}
