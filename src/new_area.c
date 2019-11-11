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
#include "common.h"

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

//
// The offset is the difference between the position of a mouse event and the
// upper left corner. For the printing we need the upper left corner.
//
static s_point offset;

#define OFFSET_NOT_SET -1

/******************************************************************************
 *
 *****************************************************************************/

static void do_print_block(const int blk_pixel_row, const int blk_pixel_col, const t_block color) {
	s_point pixel;

	for (pixel.row = blk_pixel_row; pixel.row < blk_pixel_row + size.row; pixel.row++) {
		for (pixel.col = blk_pixel_col; pixel.col < blk_pixel_col + size.col; pixel.col++) {

			if (game_area_contains(pixel.row, pixel.col)) {
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

			if (game_area_contains(pixel.row, pixel.col)) {
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
 * The function computes the offset, in case the mouse event is outside the
 * home area. The offset is the relative upper left corner of the first none
 * empty block.
 *****************************************************************************/

static void get_offset() {

	for (int row = 0; row < dim.row; row++) {
		for (int col = 0; col < dim.col; col++) {

			//
			// We are looking for the first none empty block.
			//
			if (blocks[row][col] == color_none) {
				continue;
			}

			//
			// Compute the relative upper left corner and we are done.
			//
			offset.row = row * size.row;
			offset.col = col * size.col;

			return;
		}
	}
}

/******************************************************************************
 * The function initializes the new area.
 *****************************************************************************/

void new_area_init() {

	s_point_set(&size, 2, 4);

	s_point_set(&dim, 3, 3);

	s_point_set(&offset, -1, -1);

	blocks = blocks_create(dim.row, dim.col);
}

/******************************************************************************
 * The function frees the allocated memory.
 *****************************************************************************/

void new_area_free() {

	blocks_free(blocks, dim.row);
}

/******************************************************************************
 * The function processes all blocks. For each block, the upper left pixel
 * (terminal character) is computed.
 *****************************************************************************/

void new_area_process_blocks(const bool do_print) {
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

void new_area_fill() {
	colors_init_random(blocks, dim.row, dim.col);
}

/******************************************************************************
 * The function check whether the position changed or not.
 *****************************************************************************/

bool new_area_same_pos(const int event_row, const int event_col) {

	const bool result = event_row == pos.row && event_col == pos.col;
	log_debug("Position %d/%d  is the same: %d", event_row, event_col, result);
	return result;
}

/******************************************************************************
 * The function processes a new mouse event.
 *****************************************************************************/

void new_area_process(const int event_row, const int event_col) {

	new_area_process_blocks(DO_DELETE);

	//
	// If the row or col is negative, we move to the home position.
	//
	if (event_row == HOME_ROW || event_col == HOME_COL) {
		s_point_set(&pos, home.row, home.col);

		s_point_set(&offset, OFFSET_NOT_SET, OFFSET_NOT_SET);

	} else {

		if (offset.row == OFFSET_NOT_SET && offset.col == OFFSET_NOT_SET) {

			if (is_inside_area(&pos, &dim, &size, event_row, event_col)) {
				offset.row = event_row - home.row;
				offset.col = event_col - home.col;
			} else {
				get_offset();
			}
		}

		pos.row = event_row - offset.row;
		pos.col = event_col - offset.col;

		log_debug("pos: %d/%d offset:  %d/%d", pos.row, pos.col, offset.row, offset.col);
	}

	new_area_process_blocks(DO_PRINT);
}

/******************************************************************************
 * The function returns a struct with the total size of the new area.
 *****************************************************************************/

s_point new_area_get_size() {
	return blocks_get_size(&dim, &size);
}

/******************************************************************************
 * The function sets the position of the new area. This is done on the
 * initialization and on resizing the terminal.
 *****************************************************************************/

void new_area_set_pos(const int row, const int col) {

	//
	// The home position is the initial position and changes only on resizing
	// the terminal
	//
	home.row = row;
	home.col = col;

	//
	// The initial position is the home position. It can change by the mouse
	// motion.
	//
	pos.row = home.row;
	pos.col = home.col;

	log_debug("position: %d/%d", pos.row, pos.col);
}

/******************************************************************************
 *
 *****************************************************************************/

static bool used_area_is_inside(const s_point *used_idx, const s_point *used_dim) {

	const int ul_row = pos.row + used_idx->row * size.row;
	const int ul_col = pos.col + used_idx->col * size.col;

	if (!game_area_contains(ul_row, ul_col)) {
		log_debug("used area - upper left not inside: %d/%d", ul_row, ul_col);
		return false;
	}

	const int lr_row = ul_row + (used_dim->row - 1) * size.row;
	const int lr_col = ul_col + (used_dim->col - 1) * size.col;

	if (!game_area_contains(lr_row, lr_col)) {
		log_debug("used area - lower right not inside: %d/%d", lr_row, lr_col);
		return false;
	}

	log_debug_str("used area - is inside");

	return true;
}

/******************************************************************************
 *
 *****************************************************************************/

bool new_area_is_dropped() {
	s_point idx;

	//
	// Ensure that the blocks are aligned.
	//
	if (!game_area_is_aligned(pos.row, pos.col)) {
		log_debug_str("New blocks are not aligned!");
		return false;
	}

//	if (!game_area_is_aligned(pos.row, pos.col)) {
//
//		if (game_area_is_aligned(pos.row, pos.col + 1)) {
//			pos.col++;
//
//		} else if (game_area_is_aligned(pos.row, pos.col - 1)) {
//			pos.col--;
//
//		} else {
//			log_debug_str("New blocks are not aligned!");
//			return false;
//		}
//	}

	//
	// Compute the used area of the new blocks.
	//
	s_point used_idx, used_dim;
	blocks_get_used_area(blocks, &dim, &used_idx, &used_dim);

	//
	// Ensure that the used area is inside the game area.
	//
	if (!used_area_is_inside(&used_idx, &used_dim)) {
		return false;
	}

	//
	// Compute the upper left corner of the used area.
	//
	s_point pixel;
	pixel.row = block_upper_left(pos.row, size.row, used_idx.row);
	pixel.col = block_upper_left(pos.col, size.col, used_idx.col);

	//
	// Compute the corresponding index in the game area.
	//
	game_area_get_block(&pixel, &idx);

	//
	// Check if the used area can be dropped at the game area position.
	//
	if (!game_area_drop(blocks, &idx, &used_idx, &used_dim, false)) {
		return false;
	}

	//
	// Drop the used area.
	//
	game_area_drop(blocks, &idx, &used_idx, &used_dim, true);

	//
	// Remove adjacent blocks if possible.
	//
	const int num_removed = game_area_remove_blocks(blocks, &idx, &used_idx, &used_dim);
	if (num_removed >= 4) {
		info_area_add_to_score(num_removed);
		game_area_print();
	}

	log_debug_str("Is dropped!");

	return true;
}

/******************************************************************************
 *
 *****************************************************************************/

bool new_area_can_drop() {
	s_point used_idx, used_dim;

	blocks_get_used_area(blocks, &dim, &used_idx, &used_dim);

	return game_area_can_drop_anywhere(blocks, &used_idx, &used_dim);
}

