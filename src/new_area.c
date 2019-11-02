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

#include "new_area.h"
#include "game_area.h"
#include "info_area.h"
#include "bg_area.h"

#include "blocks.h"

/******************************************************************************
 * The file operates on the following variables and structs.
 *****************************************************************************/

//
// The 2-dimensional array with the block colors.
//
static t_block **blocks;

//
// The dimensions of the 2-dimensional array.
//
static s_point dim;

//
// The current position (upper left corner) of the blocks.
//
static s_point pos;

//
// The size of a single block.
//
static s_point size;

//
// The home position (upper left corner) of the new blocks.
//
static s_point home;

/******************************************************************************
 *
 *****************************************************************************/

static void do_print_block(const int blk_pixel_row, const int blk_pixel_col, const t_block color) {
	s_point pixel;

	for (pixel.row = blk_pixel_row; pixel.row < blk_pixel_row + size.row; pixel.row++) {
		for (pixel.col = blk_pixel_col; pixel.col < blk_pixel_col + size.col; pixel.col++) {

			if (game_area_contains(&pixel)) {
				game_area_print_pixel(&pixel, color);

			} else if (info_area_contains(&pixel)) {
				info_area_print_pixel(&pixel, color);

			} else {
				bg_area_print_pixel(&pixel, color, BLOCK_FULL);
			}
		}
	}
}

/******************************************************************************
 *
 *****************************************************************************/

static void do_delete_block(const int blk_pixel_row, const int blk_pixel_col, const t_block color) {
	s_point pixel;

	for (pixel.row = blk_pixel_row; pixel.row < blk_pixel_row + size.row; pixel.row++) {
		for (pixel.col = blk_pixel_col; pixel.col < blk_pixel_col + size.col; pixel.col++) {

			if (game_area_contains(&pixel)) {
				game_area_print_pixel(&pixel, color_none);

			} else if (info_area_contains(&pixel)) {
				info_area_print_pixel(&pixel, color_none);

			} else {
				bg_area_print_pixel(&pixel, color, BLOCK_EMPTY);
			}
		}
	}
}

/******************************************************************************
 * The function processes all blocks. For each block, the upper left pixel
 * (terminal character) is computed.
 *****************************************************************************/

//
//   do_process_blocks(DO_DELETE); vs do_process_blocks(false);
//
#define DO_PRINT true

#define DO_DELETE false

static void do_process_blocks(const bool do_print) {
	int pixel_row, pixel_col;

	t_block color;

	for (int row = 0; row < dim.row; row++) {
		for (int col = 0; col < dim.col; col++) {

			if (blocks[row][col] == color_none) {
				continue;
			}

			pixel_row = block_upper_left(pos.row, size.row, row);
			pixel_col = block_upper_left(pos.col, size.col, col);

			color = blocks[row][col];

			if (do_print) {
				do_print_block(pixel_row, pixel_col, color);

			} else {
				do_delete_block(pixel_row, pixel_col, color);
			}
		}
	}
}

/******************************************************************************
 *
 *****************************************************************************/

void new_area_init() {

	s_point_set(&home, 10, 60);

	s_point_set(&pos, home.row, home.col);

	s_point_set(&size, 2, 4);

	s_point_set(&dim, 3, 3);

	blocks = blocks_create(dim.row, dim.col);
}

/******************************************************************************
 * The function frees the allocated memory.
 *****************************************************************************/

void new_area_free() {

	blocks_free(blocks, dim.row);
}

/******************************************************************************
 *
 *****************************************************************************/

void new_area_next() {

	s_point_set(&pos, home.row, home.col);

	colors_init_random(blocks, dim.row, dim.col);

	do_process_blocks(DO_PRINT);
}

/******************************************************************************
 * The function processes a new mouse event.
 *****************************************************************************/

void new_area_process(const int row, const int col) {

	//
	// If the position of the mouse event is the same as before, we do not have
	// to do anything.
	//
	if (row == pos.row && col == pos.col) {
		log_debug("Position is the same - row: %d col: %d", row, col);
		return;
	}

	log_debug("Processing: row: %d col: %d", row, col);

	do_process_blocks(DO_DELETE);

	//
	// If the row or col is negative, we move to the home position.
	//
	if (row == HOME_ROW || col == HOME_COL) {
		s_point_set(&pos, home.row, home.col);

	} else {
		s_point_set(&pos, row, col);
	}

	do_process_blocks(DO_PRINT);
}
