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
#include <unistd.h>
#include <errno.h>
#include <string.h>

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

//#define USLEEP 200000

#define USLEEP 2000000

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

static s_area _drop_area;

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
 *
 *****************************************************************************/
// TODO:
static void game_area_print_pixel(const s_area *game_area, const s_point *pixel, const t_block fg_color) {

	log_debug("pixel: %d/%d, color: %d", pixel->row, pixel->col, fg_color);

	const s_point idx = s_area_get_block(game_area, pixel);

	const t_block bg_color = game_area->blocks[idx.row][idx.col];

	colors_game_attr(fg_color, bg_color, colors_is_even(idx.row, idx.col));

	//
	// Get the character to be printed. This can be a space, a block or
	// transparent block character.
	//

	const wchar_t chr = colors_get_char(fg_color, bg_color);

	mvprintw(pixel->row, pixel->col, "%lc", chr);
}

// TODO: Mark OK ----- LOWER --------------------------------------------------

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
 * The function processes all blocks. For each block, the upper left pixel
 * (terminal character) is computed.
 *****************************************************************************/

static void drop_area_process_blocks(const s_area *game_area, const s_area *drop_area, const bool do_print) {
	s_point drop_area_ul;

	for (int row = 0; row < drop_area->dim.row; row++) {
		for (int col = 0; col < drop_area->dim.col; col++) {

			//
			// If the drop area block is empty, there is nothing to do.
			//
			if (drop_area->blocks[row][col] == CLR_NONE) {
				continue;
			}

			//
			// Compute the upper left corner of the current block of the drop
			// area.
			//
			drop_area_ul.row = block_upper_left(drop_area->pos.row, drop_area->size.row, row);
			drop_area_ul.col = block_upper_left(drop_area->pos.col, drop_area->size.col, col);

			if (do_print) {
				game_print_foreground(game_area, &drop_area_ul, &drop_area->size, drop_area->blocks[row][col], BLOCK_FULL);

			} else {
				game_print_foreground(game_area, &drop_area_ul, &drop_area->size, CLR_NONE, BLOCK_EMPTY);
			}
		}
	}
}

/******************************************************************************
 * The function tries the adjust the drop area, which means moving it one
 * character to the left or right.
 *****************************************************************************/

static bool do_adjust(const s_area *game_area, const s_area *drop_area, s_area *adj_area) {

	if (s_area_is_aligned(game_area, drop_area->pos.row, drop_area->pos.col)) {
		return true;
	}

	if (s_area_is_aligned(game_area, drop_area->pos.row, drop_area->pos.col + ADJUST)) {
		adj_area->pos.col += ADJUST;
		return true;
	}

	if (s_area_is_aligned(game_area, drop_area->pos.row, drop_area->pos.col - ADJUST)) {
		adj_area->pos.col -= ADJUST;
		return true;
	}

	log_debug_str("New blocks are not aligned!");
	return false;
}

/******************************************************************************
 * The function copies one area to an other. The blocks are shared.
 *****************************************************************************/

static void s_area_copy(s_area *to, const s_area *from) {

	to->blocks = from->blocks;

	s_point_copy(&to->dim, &from->dim);
	s_point_copy(&to->pos, &from->pos);
	s_point_copy(&to->size, &from->size);
}

/******************************************************************************
 * The function initializes the drop area. The area is filled with new colors.
 * As a second step, the area is normalized, which means that completely empty
 * rows and columns are removed from the beginning of the area
 *****************************************************************************/

static void area_update(s_area *area) {

	//
	// Fill the blocks with random colors.
	//
	colors_init_random(_drop_area.blocks, _drop_area.dim.row, _drop_area.dim.col);

	//
	// Normalize the area.
	//
	s_area_normalize(area);
}

/******************************************************************************
 * The function moves the drop area to a given position.
 *****************************************************************************/

static void animate_move(s_area *game_area, s_area *drop_area, const s_point *to) {

	log_debug("Move from: %d/%d to: %d/%d", drop_area->pos.row, drop_area->pos.col, to->row, to->col);

	//
	// If the drop area is already at the position, there is nothing to do.
	//
	if (s_point_same(&drop_area->pos, to)) {
		return;
	}

	//
	// Delete the drop area at the old position.
	//
	drop_area_process_blocks(&_game_area, &_drop_area, DO_DELETE);

	//
	// Move the drop area to the new position.
	//
	s_point_copy(&drop_area->pos, to);

	//
	// Print and show the drop area at the new position.
	//
	drop_area_process_blocks(game_area, &_drop_area, DO_PRINT);

	refresh();

	//
	// Sleep to show the movement
	//
	if (usleep(USLEEP) == -1) {
		log_exit("Sleep failed: %s", strerror(errno));
	}
}

/******************************************************************************
 * The function drops the drop area at a given position.
 *****************************************************************************/

static void animate_drop(s_area *game_area, const s_point *drop_point, s_area *drop_area) {

	//
	// Drop the used area.
	//
	s_area_drop(game_area, drop_point, drop_area, true);

	//
	// Delete the drop area from the foreground
	//
	drop_area_process_blocks(&_game_area, &_drop_area, DO_DELETE);

	refresh();

	//
	// Sleep to show the movement
	//
	if (usleep(USLEEP) == -1) {
		log_exit("Sleep failed: %s", strerror(errno));
	}
}

// TODO: Mark OK ----- UPPER --------------------------------------------------

/******************************************************************************
 * The function processes a new mouse event.
 *****************************************************************************/
// TODO: pickup drop area only in the home position.
static void new_area_process(s_area *game_area, s_area *drop_area, const int event_row, const int event_col) {

	//
	// If the row or col is negative, we move to the home position.
	//
	if (event_row == HOME_ROW || event_col == HOME_COL) {

		s_point_set(&drop_area->pos, _home.row, _home.col);

		s_point_set(&_offset, OFFSET_NOT_SET, OFFSET_NOT_SET);

	} else {

		if (_offset.row == OFFSET_NOT_SET && _offset.col == OFFSET_NOT_SET) {
			if (s_area_is_inside(drop_area, event_row, event_col)) {
				_offset.row = event_row - _home.row;
				_offset.col = event_col - _home.col;

			} else {

				//
				// The offset is the relative position of the upper left corner
				// of the used area, from the new area.
				//
				s_point_set(&_offset, 0, 0);
			}
		}

		drop_area->pos.row = event_row - _offset.row;
		drop_area->pos.col = event_col - _offset.col;

		log_debug("pos: %d/%d offset:  %d/%d", drop_area->pos.row, drop_area->pos.col, _offset.row, _offset.col);
	}

	drop_area_process_blocks(game_area, drop_area, DO_PRINT);
}

/******************************************************************************
 *
 *****************************************************************************/

static bool new_area_is_dropped(s_area *game_area, s_area *drop_area, t_block **marks) {

	//
	// Copy the drop area to the adjusted area.
	//
	s_area adj_area;
	s_area_copy(&adj_area, drop_area);

	//
	// do adjust the new_area if possible
	//
	if (!do_adjust(game_area, drop_area, &adj_area)) {
		return false;
	}

	//
	// Ensure that the used area is inside the game area.
	//
	if (!s_area_used_area_is_inside(game_area, &adj_area)) {
		return false;
	}

	//
	// Compute the corresponding index in the game area.
	//
	const s_point drop_point = s_area_get_block(game_area, &adj_area.pos);

	//
	// Check if the used area can be dropped at the game area position.
	//
	if (!s_area_drop(game_area, &drop_point, drop_area, false)) {
		return false;
	}

	log_debug("new: %d/%d adjust: %d/%d", drop_area->pos.row, drop_area->pos.col, adj_area.pos.row, adj_area.pos.col);

	animate_move(game_area, drop_area, &adj_area.pos);

	animate_drop(game_area, &drop_point, drop_area);

	//
	// Remove adjacent blocks if possible.
	//
	const int num_removed = s_area_remove_blocks(game_area, &drop_point, drop_area, marks);
	if (num_removed >= 4) {
		info_area_add_to_score(num_removed);
		game_area_print(game_area);
	}

	log_debug_str("Is dropped!");

	return true;
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
	s_area_create(&_drop_area, 3, 3, 2, 4);

	//
	// Fill the blocks with random colors.
	//
	area_update(&_drop_area);
	//colors_init_random(_new_area.blocks, _new_area.dim.row, _new_area.dim.col);

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

	s_area_free(&_drop_area);
}

/******************************************************************************
 *
 *****************************************************************************/

void game_process_event_pressed(const int row, const int col) {

	if (!s_area_same_pos(&_drop_area, row, col)) {

		drop_area_process_blocks(&_game_area, &_drop_area, DO_DELETE);

		new_area_process(&_game_area, &_drop_area, row, col);
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
	if (new_area_is_dropped(&_game_area, &_drop_area, _marks)) {

		//
		// Delete the current none-empty blocks. After calling
		// new_area_fill(), the non-empty blocks may be different.
		//
		//new_area_delete(&_game_area, &_drop_area);
		drop_area_process_blocks(&_game_area, &_drop_area, DO_DELETE);

		//
		// Fill the blocks with new, random colors.
		//
		area_update(&_drop_area);
		//colors_init_random(_new_area.blocks, _new_area.dim.row, _new_area.dim.col);

		if (!s_area_can_drop_anywhere(&_game_area, &_drop_area)) {
			log_debug_str("ENDDDDDDDDDDDDD");
			info_area_set_msg("End");
		}
	} else {
		//new_area_delete(&_game_area, &_drop_area);
		drop_area_process_blocks(&_game_area, &_drop_area, DO_DELETE);
	}

	//
	// Move the current drop area to the home position or create a new drop
	// position at the home position.
	//
	// TODO: drop only if it is possible,
	new_area_process(&_game_area, &_drop_area, HOME_ROW, HOME_COL);
}

// -------------------------------
#define DELIM 4

void game_do_center() {

	//
	// Get the sizes of the different areas.
	//
	const s_point game_size = s_area_get_size(&_game_area);

	const s_point new_size = s_area_get_size(&_drop_area);

	const s_point info_size = info_area_get_size();

	//
	// Compute the size of all areas.
	//
	int total_rows = game_size.row;
	int total_cols = game_size.col + DELIM + max(info_size.col, new_size.col);

	//
	// Get the center positions
	//
	const int ul_row = (getmaxy(stdscr) - total_rows) / 2;
	const int ul_col = (getmaxx(stdscr) - total_cols) / 2;

	log_debug("upper left row: %d col: %d", ul_row, ul_col);

	s_point_set(&_game_area.pos, ul_row, ul_col);

	s_point_set(&_home, ul_row + info_size.row + DELIM, ul_col + game_size.col + DELIM);
	s_point_set(&_drop_area.pos, _home.row, _home.col);

	info_area_set_pos(ul_row, _home.col);

	//
	// Delete the old content.
	//
	erase();

	//
	// Print the areas at the updated position
	//
	game_area_print(&_game_area);

	drop_area_process_blocks(&_game_area, &_drop_area, DO_PRINT);

	info_area_print();

	//
	// Call refresh to show the result
	//
	refresh();
}
