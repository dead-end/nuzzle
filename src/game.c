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

#include "game.h"

#include "info_area.h"
#include "bg_area.h"

#include "blocks.h"
#include "common.h"

#include "s_area.h"

#include "colors.h"

#define HOME_ROW -1
#define HOME_COL -1

#define DO_PRINT true
#define DO_DELETE false

/******************************************************************************
 *
 *****************************************************************************/

//
// The home position (upper left corner) of the new blocks.
//
static s_point _home;

//
// The offset is the difference between the position of a mouse event and the
// upper left corner. For the printing we need the upper left corner.
//
static s_point _offset;

#define OFFSET_NOT_SET -1

#define ADJUST 1

static s_area _new_area;

static s_area _game_area;

#define GAME_SIZE 11

/******************************************************************************
 *
 *****************************************************************************/

//
// The 2-dimensional array with temporary data.
//
static t_block **_marks;

/******************************************************************************
 * The function prints the game area with no foreground color. This can be used
 * as an initialization.
 *****************************************************************************/

static void game_area_print(const s_area *game_area) {

	//
	// Iterate through the blocks of the game area.
	//
	for (int row = 0; row < game_area->dim.row; row++) {
		for (int col = 0; col < game_area->dim.col; col++) {

			colors_game_attr(CLR_NONE, game_area->blocks[row][col], colors_is_even(row, col));

			s_area_print_block(game_area, row, col, BLOCK_EMPTY);
		}
	}
}

/******************************************************************************
 * The function determines the character to display for a pixel (terminal
 * character).
 *****************************************************************************/

static wchar_t get_char(const s_area *game_area, const s_point *idx, const t_block fg_color) {
	wchar_t chr;

	if (fg_color != CLR_NONE) {

		if (game_area->blocks[idx->row][idx->col] != CLR_NONE) {
			chr = BLOCK_BOTH;

		} else {
			chr = BLOCK_FULL;
		}
	} else {
		chr = BLOCK_EMPTY;
	}

	log_debug("block: (%d, %d) color: %d char '%lc", idx->row, idx->col, fg_color, chr);

	return chr;
}

/******************************************************************************
 *
 *****************************************************************************/
// TODO:
static void game_area_print_pixel(const s_area *game_area, const s_point *pixel, const t_block fg_color) {

	log_debug("pixel: %d/%d, color: %d", pixel->row, pixel->col, fg_color);

	const s_point idx = s_area_get_block(game_area, pixel);

	colors_game_attr(fg_color, game_area->blocks[idx.row][idx.col], colors_is_even(idx.row, idx.col));

	const wchar_t chr = get_char(game_area, &idx, fg_color);

	mvprintw(pixel->row, pixel->col, "%lc", chr);
}

// ----------------------------------------------------------------------------

/******************************************************************************
 * The function prints an area with a foreground character and color. If the
 * character is an empty block and the color is none, the foreground of the
 * area is deleted.
 *****************************************************************************/

static void game_print_foreground(const s_area *game_area, const s_point *area_pos, const s_point *area_size, const t_block color, const wchar_t chr) {
	s_point pixel;

	for (pixel.row = area_pos->row; pixel.row < area_pos->row + area_size->row; pixel.row++) {
		for (pixel.col = area_pos->col; pixel.col < area_pos->col + area_size->col; pixel.col++) {

			if (s_area_is_inside(game_area, pixel.row, pixel.col)) {
				game_area_print_pixel(game_area, &pixel, color);

			} else if (info_area_contains(&pixel)) {
				info_area_print_pixel(&pixel, color);

			} else {
				bg_area_print_pixel(&pixel, color, chr);
			}
		}
	}
}

/******************************************************************************
 * The function removes the foreground character in an area around the new
 * area. If the block is not aligned at the time of dropping, the position is
 * adjusted. This adjustment is added on both sides of the area. This shifts
 * the position to the left and increases the size with twice the ADJUST size.
 *****************************************************************************/

static void new_area_delete(const s_area *game_area, const s_area *new_area) {
	s_point area_pos = { .row = new_area->pos.row, .col = new_area->pos.col - ADJUST };
	s_point area_size = { .row = new_area->size.row * new_area->dim.row, .col = new_area->size.col * new_area->dim.col + 2 * ADJUST };

	game_print_foreground(game_area, &area_pos, &area_size, CLR_NONE, BLOCK_EMPTY);
}

/******************************************************************************
 * The function processes all blocks. For each block, the upper left pixel
 * (terminal character) is computed.
 *****************************************************************************/

static void new_area_process_blocks(const s_area *game_area, s_area *new_area, const bool do_print) {
	s_point na_upper_left;

	for (int row = 0; row < new_area->dim.row; row++) {
		for (int col = 0; col < new_area->dim.col; col++) {

			if (new_area->blocks[row][col] == CLR_NONE) {
				continue;
			}

			na_upper_left.row = block_upper_left(new_area->pos.row, new_area->size.row, row);
			na_upper_left.col = block_upper_left(new_area->pos.col, new_area->size.col, col);

			if (do_print) {
				game_print_foreground(game_area, &na_upper_left, &new_area->size, new_area->blocks[row][col], BLOCK_FULL);

			} else {
				game_print_foreground(game_area, &na_upper_left, &new_area->size, CLR_NONE, BLOCK_EMPTY);
			}
		}
	}
}

/******************************************************************************
 * The function processes a new mouse event.
 *****************************************************************************/

static void new_area_process(s_area *game_area, s_area *new_area, const int event_row, const int event_col) {

	//
	// If the row or col is negative, we move to the home position.
	//
	if (event_row == HOME_ROW || event_col == HOME_COL) {

		s_point_set(&new_area->pos, _home.row, _home.col);

		s_point_set(&_offset, OFFSET_NOT_SET, OFFSET_NOT_SET);

	} else {

		if (_offset.row == OFFSET_NOT_SET && _offset.col == OFFSET_NOT_SET) {
			if (s_area_is_inside(new_area, event_row, event_col)) {
				_offset.row = event_row - _home.row;
				_offset.col = event_col - _home.col;

			} else {

				//
				// Get the used area. The index of the used area is the index
				// of the upper left corner.
				//
				s_used_area used_area;
				s_area_get_used_area(new_area, &used_area);

				//
				// The offset is the relative position of the upper left corner
				// of the used area, from the new area.
				//
				s_point_set(&_offset, used_area.idx.row * new_area->size.row, used_area.idx.col * new_area->size.col);
			}
		}

		new_area->pos.row = event_row - _offset.row;
		new_area->pos.col = event_col - _offset.col;

		log_debug("pos: %d/%d offset:  %d/%d", new_area->pos.row, new_area->pos.col, _offset.row, _offset.col);
	}

	new_area_process_blocks(game_area, new_area, DO_PRINT);
}

/******************************************************************************
 *
 *****************************************************************************/

static bool do_adjust(s_area *game_area, s_area *new_area) {

	if (s_area_is_aligned(game_area, new_area->pos.row, new_area->pos.col)) {
		return true;
	}

	if (s_area_is_aligned(game_area, new_area->pos.row, new_area->pos.col + ADJUST)) {
		new_area->pos.col += ADJUST;
		return true;
	}

	if (s_area_is_aligned(game_area, new_area->pos.row, new_area->pos.col - ADJUST)) {
		new_area->pos.col -= ADJUST;
		return true;
	}

	log_debug_str("New blocks are not aligned!");
	return false;
}

/******************************************************************************
 * The function computes the upper left corner of the used area.
 *****************************************************************************/

static s_point s_used_area_ul_corner(const s_used_area *used_area) {
	s_point result;

	result.row = used_area->area->pos.row + used_area->area->size.row * used_area->idx.row;
	result.col = used_area->area->pos.col + used_area->area->size.col * used_area->idx.col;

	return result;
}

/******************************************************************************
 *
 *****************************************************************************/

static bool new_area_is_dropped(s_area *game_area, s_area *new_area, t_block **marks) {

	//
	// do adjust the new_area if possible
	//
	if (!do_adjust(game_area, new_area)) {
		return false;
	}

	//
	// Compute the used area of the new blocks.
	//
	s_used_area used_area;
	s_area_get_used_area(new_area, &used_area);

	//
	// Ensure that the used area is inside the game area.
	//
	if (!s_area_used_area_is_inside(game_area, &used_area)) {
		return false;
	}

	//
	// Compute the upper left corner of the used area.
	//
	const s_point ul_used_area = s_used_area_ul_corner(&used_area);

	//
	// Compute the corresponding index in the game area.
	//
	const s_point idx = s_area_get_block(game_area, &ul_used_area);

	//
	// Check if the used area can be dropped at the game area position.
	//
	if (!s_area_drop(game_area, &idx, &used_area, false)) {
		return false;
	}

	//
	// Drop the used area.
	//
	s_area_drop(game_area, &idx, &used_area, true);

	//
	// Remove adjacent blocks if possible.
	//
	const int num_removed = s_area_remove_blocks(game_area, &idx, &used_area, marks);
	if (num_removed >= 4) {
		info_area_add_to_score(num_removed);
		game_area_print(game_area);
	}

	log_debug_str("Is dropped!");

	return true;
}

/******************************************************************************
 *
 *****************************************************************************/

static bool new_area_can_drop_anywhere(s_area *game_area, s_area *new_area) {

	s_used_area used_area;

	s_area_get_used_area(new_area, &used_area);

	return s_area_can_drop_anywhere(game_area, &used_area);
}

// ----------------------------------------
// INTERFACE

/******************************************************************************
 * The function initializes the new area.
 *****************************************************************************/

void game_init() {

	//
	// Game area
	//
	s_area_create(&_game_area, GAME_SIZE, GAME_SIZE, 2, 4);

	blocks_set(_game_area.blocks, &_game_area.dim, CLR_NONE);

	log_debug("game_area pos: %d/%d", _game_area.pos.row, _game_area.pos.col);

	//
	// Marks
	//
	_marks = blocks_create(_game_area.dim.row, _game_area.dim.col);

	//
	// new area
	//
	s_area_create(&_new_area, 3, 3, 2, 4);

	//
	// Fill the blocks with random colors.
	//
	colors_init_random(_new_area.blocks, _new_area.dim.row, _new_area.dim.col);

	//
	// rest
	//
	s_point_set(&_offset, -1, -1);
}

/******************************************************************************
 * The function frees the allocated memory.
 *****************************************************************************/

void game_free() {

	log_debug_str("Freeing blocks.");

	s_area_free(&_game_area);

	blocks_free(_marks, _game_area.dim.row);

	s_area_free(&_new_area);
}

/******************************************************************************
 *
 *****************************************************************************/

s_point game_get_game_area_size() {
	return s_area_get_size(&_game_area);
}

/******************************************************************************
 * The function returns a struct with the total size of the new area.
 *****************************************************************************/

s_point game_get_new_area_size() {
	return s_area_get_size(&_new_area);
}

/******************************************************************************
 *
 *****************************************************************************/

void game_set_game_area_pos(const int row, const int col) {
	s_point_set(&_game_area.pos, row, col);
}

/******************************************************************************
 * The function sets the position of the new area. This is done on the
 * initialization and on resizing the terminal.
 *****************************************************************************/

void game_set_new_area_pos(const int row, const int col) {

	//
	// The home position is the initial position and changes only on resizing
	// the terminal
	//
	s_point_set(&_home, row, col);

	//
	// The initial position is the home position. It can change by the mouse
	// motion.
	//
	s_point_set(&_new_area.pos, row, col);

	log_debug("position: %d/%d", _new_area.pos.row, _new_area.pos.col);
}

/******************************************************************************
 *
 *****************************************************************************/

void game_print_game_area() {
	game_area_print(&_game_area);
}

/******************************************************************************
 *
 *****************************************************************************/

void game_print_new_area() {
	new_area_process_blocks(&_game_area, &_new_area, DO_PRINT);
}

/******************************************************************************
 *
 *****************************************************************************/

void game_process_event_pressed(const int row, const int col) {

	if (!s_area_same_pos(&_new_area, row, col)) {

		new_area_process_blocks(&_game_area, &_new_area, DO_DELETE);

		new_area_process(&_game_area, &_new_area, row, col);
	}
}

/******************************************************************************
 *
 *****************************************************************************/

void game_process_event_release(const int row, const int col) {
	log_debug("pos: %d/%d", row, col);

	//
	// Dropping means that the game area is updated. If the new
	// area is deleted, which means that the foreground is deleted,
	// the background will be visible.
	//
	if (new_area_is_dropped(&_game_area, &_new_area, _marks)) {

		//
		// Delete the current none-empty blocks. After calling
		// new_area_fill(), the non-empty blocks may be different.
		//
		new_area_delete(&_game_area, &_new_area);

		//
		// Fill the blocks with new, random colors.
		//
		colors_init_random(_new_area.blocks, _new_area.dim.row, _new_area.dim.col);

		if (!new_area_can_drop_anywhere(&_game_area, &_new_area)) {
			log_debug_str("ENDDDDDDDDDDDDD");
			info_area_set_msg("End");
		}
	} else {
		new_area_delete(&_game_area, &_new_area);
	}

	new_area_process(&_game_area, &_new_area, HOME_ROW, HOME_COL);
}
