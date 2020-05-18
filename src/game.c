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
#include <nz_curses.h>
#include <string.h>

#include "nuzzle.h"
#include "game.h"
#include "info_area.h"
#include "bg_area.h"
#include "blocks.h"
#include "common.h"
#include "s_area.h"
#include "colors.h"
#include "s_status.h"

#define DO_PRINT true

#define DO_DELETE false

#define USLEEP 200000

//#define USLEEP 2000000

/******************************************************************************
 *
 *****************************************************************************/

#define HOME_EVENT -1

#define OFFSET_NOT_SET -1

#define ADJUST 1

#define GAME_SIZE 11

#define DROP_AREA_ROW 3

#define DROP_AREA_COL 3

/******************************************************************************
 * The structs for the game.
 *****************************************************************************/

//
// The game area struct
//
static s_area _game_area;

//
// The drop area struct
//
static s_area _drop_area;

//
// The 2-dimensional array with temporary data.
//
static t_block **_marks;

//
// The home position (upper left corner) of the new blocks.
//
static s_point _home;

static WINDOW *_win_game = NULL;

/******************************************************************************
 * The function prints the game area with no drop area. This can be used as an
 * initialization.
 *****************************************************************************/

static void game_area_print(WINDOW *win, const s_area *game_area) {
	wchar_t chr;
	t_block ga_color;

	//
	// Iterate through the blocks of the game area.
	//
	for (int row = 0; row < game_area->dim.row; row++) {
		for (int col = 0; col < game_area->dim.col; col++) {

			ga_color = game_area->blocks[row][col];

			//
			// Set the color pair and get the character to display.
			//
			chr = colors_chess_attr_char(win, ga_color, CLR_NONE, colors_is_even(row, col));

			//
			// Print the block with a given color and character.
			//
			s_area_print_block(win, game_area, row, col, chr);
		}
	}
}

/******************************************************************************
 * The function prints a pixel with a given drop area color. The pixel is
 * inside the game area.
 *****************************************************************************/

static void game_area_print_pixel(WINDOW *win, const s_area *game_area, const s_point *pixel, const t_block da_color) {

	log_debug("pixel: %d/%d, fg-color: %d", pixel->row, pixel->col, da_color);

	//
	// Get the background color of the pixel. We need the corresponding game
	// block, which contains the color.
	//
	s_point block_idx;
	s_area_get_block(game_area, pixel, &block_idx);
	const t_block ga_color = game_area->blocks[block_idx.row][block_idx.col];

	//
	// Set the color pair and get the character to display.
	//
	const wchar_t chr = colors_chess_attr_char(win, ga_color, da_color, colors_is_even(block_idx.row, block_idx.col));

	//
	// Print the character at the position.
	//
	mvwprintw(win, pixel->row, pixel->col, "%lc", chr);
}

/******************************************************************************
 * The function prints a block of a drop area at a given position, with a given
 * color. The block may overlap areas (game_area, info_area, background area).
 *****************************************************************************/

static void game_print_foreground(WINDOW *win, const s_area *game_area, const s_point *drop_area_pos, const s_point *drop_area_size, const t_block da_color) {
	s_point pixel;

	//
	// Process the pixels of the block.
	//
	for (pixel.row = drop_area_pos->row; pixel.row < drop_area_pos->row + drop_area_size->row; pixel.row++) {
		for (pixel.col = drop_area_pos->col; pixel.col < drop_area_pos->col + drop_area_size->col; pixel.col++) {

			//
			// Check the position of each block pixel.
			//
			if (s_area_is_inside(game_area, pixel.row, pixel.col)) {
				game_area_print_pixel(win, game_area, &pixel, da_color);

			} else if (info_area_contains(&pixel)) {
				info_area_print_pixel(win, &pixel, da_color);

			} else {
				bg_area_print_pixel(win, &pixel, da_color);
			}
		}
	}
}

/******************************************************************************
 * The function processes all blocks. For each block, the upper left pixel
 * (terminal character) is computed.
 *****************************************************************************/

static void drop_area_process_blocks(WINDOW *win, const s_area *game_area, const s_area *drop_area, const bool do_print) {
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
				game_print_foreground(win, game_area, &drop_area_ul, &drop_area->size, drop_area->blocks[row][col]);

			} else {
				game_print_foreground(win, game_area, &drop_area_ul, &drop_area->size, CLR_NONE);
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
 * The function initializes the drop area. The area is filled with new colors.
 * As a second step, the area is normalized, which means that completely empty
 * rows and columns are removed from the beginning of the area
 *****************************************************************************/

static void area_update(s_area *area) {

	//
	// The normalization (see below) may change the dimension, so it has to be
	// set each time the drop area is filled.
	//
	s_point_set(&area->dim, DROP_AREA_ROW, DROP_AREA_COL);

	//
	// Fill the blocks with random colors.
	//
	colors_init_random(area->blocks, area->dim.row, area->dim.col);

	//
	// Normalize the area.
	//
	s_area_normalize(area);
}

/******************************************************************************
 * The function moves the drop area to the home position. The old position is
 * not deleted. This may be necessary to do upfront.
 *****************************************************************************/

static void drop_area_move_home(WINDOW *win, s_area *game_area, s_area *drop_area, const s_point *home, s_status *status) {

	s_point_set(&drop_area->pos, home->row, home->col);

	s_status_release(status);

	drop_area_process_blocks(win, game_area, drop_area, DO_PRINT);
}

/******************************************************************************
 * The function moves the drop area to a given position.
 *****************************************************************************/

static void animate_move(WINDOW *win, s_area *game_area, s_area *drop_area, const s_point *to) {

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
	drop_area_process_blocks(win, &_game_area, &_drop_area, DO_DELETE);

	//
	// Move the drop area to the new position.
	//
	s_point_copy(&drop_area->pos, to);

	//
	// Print and show the drop area at the new position.
	//
	drop_area_process_blocks(win, game_area, &_drop_area, DO_PRINT);

	nzc_win_refresh(win);

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

static void animate_drop(WINDOW *win, s_area *game_area, const s_point *drop_point, s_area *drop_area) {

	log_debug("Dropping drop area: %d/%d at game: %d/%d", drop_area->pos.row, drop_area->pos.col, drop_point->row, drop_point->col);

	//
	// Drop the used area.
	//
	s_area_drop(game_area, drop_point, drop_area, true);

	//
	// Delete the drop area from the foreground
	//
	drop_area_process_blocks(win, &_game_area, &_drop_area, DO_DELETE);

	nzc_win_refresh(win);

	//
	// Sleep to show the movement
	//
	if (usleep(USLEEP) == -1) {
		log_exit("Sleep failed: %s", strerror(errno));
	}
}

/******************************************************************************
 * The function checks if the drop area can be dropped. The dropping may
 * require that the position of the drop area has to be adjusted. The adjusted
 * drop area is stored in the adj_area struct. The drop_point struct is set to
 * the block index in the game area, where the drop area is dropped.
 *****************************************************************************/

static bool game_area_can_drop(s_area *game_area, s_point *drop_point, s_area *drop_area, s_area *adj_area) {

	//
	// Copy the drop area to the adjusted area.
	//
	s_area_copy(adj_area, drop_area);

	//
	// do adjust the new_area if possible
	//
	if (!do_adjust(game_area, drop_area, adj_area)) {
		return false;
	}

	log_debug("drop: %d/%d adjust: %d/%d", drop_area->pos.row, drop_area->pos.col, adj_area->pos.row, adj_area->pos.col);

	//
	// Ensure that the used area is inside the game area.
	//
	if (!s_area_is_area_inside(game_area, adj_area)) {
		return false;
	}

	//
	// Compute the corresponding index in the game area.
	//
	s_area_get_block(game_area, &adj_area->pos, drop_point);

	//
	// Check if the used area can be dropped at the game area position.
	//
	return s_area_drop(game_area, drop_point, drop_area, false);
}

// ----------------------------------------
// INTERFACE

/******************************************************************************
 * The function processes a new "released" event. This requires checking if the
 * drop area can be dropped, and if so, if the game ended.
 *****************************************************************************/

void game_process_event_release(s_status *status) {

	//
	// Maybe the drop area position has to be adjusted.
	//
	s_area adj_area;

	//
	// The block index of the upper left corner of the game, where the drop
	// area is dropped.
	//
	s_point drop_point;

	//
	// The button was released, so we check if we can drop the drop area.
	//
	if (game_area_can_drop(&_game_area, &drop_point, &_drop_area, &adj_area)) {

		//
		// Move the drop area to the adjusted position if necessary.
		//
		animate_move(_win_game, &_game_area, &_drop_area, &adj_area.pos);

		//
		// Drop the drop area.
		//
		animate_drop(_win_game, &_game_area, &drop_point, &_drop_area);

		//
		// Remove adjacent blocks if possible.
		//
		const int num_removed = s_area_remove_blocks(&_game_area, &drop_point, &_drop_area, _marks);
		if (num_removed >= 4) {
			info_area_add_to_score(_win_game, num_removed);
			game_area_print(_win_game, &_game_area);
		}

		//
		// Dropping the drop area means copying the blocks to the background.
		// After this, the drop area can be deleted from the foreground.
		//
		drop_area_process_blocks(_win_game, &_game_area, &_drop_area, DO_DELETE);

		//
		// Fill the blocks with new, random colors.
		//
		area_update(&_drop_area);

		if (!s_area_can_drop_anywhere(&_game_area, &_drop_area)) {

			//
			// If there is no place to drop it, we finished.
			//
			s_status_set_end(status);
			info_area_set_msg(_win_game, "END!", status);
			log_debug_str("ENDDDDDDDDDDDDD");
		}
	}

	//
	// If the drop area was not dropped, we simply delete the drop area at the
	// old position. It will be printed at the home position.
	//
	else {
		drop_area_process_blocks(_win_game, &_game_area, &_drop_area, DO_DELETE);
	}

	//
	// Move the current drop area to the home position or create a new drop
	// position at the home position.
	//
	drop_area_move_home(_win_game, &_game_area, &_drop_area, &_home, status);
}

/******************************************************************************
 * The function processes a new "pressed" event for the drop area. If the
 * position changed the drop area on the old position is deleted and it is
 * printed at the new position. If the drop area is not picked up it will be
 * picked up.
 *****************************************************************************/

void game_process_event_pressed(s_status *status, const int event_row, const int event_col) {

	//
	// Check if current game has ended
	//
	if (s_status_is_end(status)) {
		log_debug_str("Ignoring event due to current game end!");
		return;
	}

	log_debug("Event pos: %d/%d", event_row, event_col);

	//
	// If the position did not changed, we do nothing.
	//
	if (s_area_same_pos(&_drop_area, event_row, event_col)) {
		return;
	}

	drop_area_process_blocks(_win_game, &_game_area, &_drop_area, DO_DELETE);

	//
	// If the offset is negative, then the drop area was not picked up. In this
	// case we do the pickup.
	//
	if (!s_status_is_picked_up(status)) {

		//
		// If the event is inside the home area we compute the exact position.
		//
		if (s_area_is_inside(&_drop_area, event_row, event_col)) {

			s_status_pickup(status, event_row - _home.row, event_col - _home.col);
		}

		//
		// If the event occurs outside the home area, we use the upper left
		// corner.
		//
		else {

			//
			// Mark as picked up with offset 0/0
			//
			s_status_pickup(status, 0, 0);
		}
	}

	//
	// Compute the next position with the event and the offset.
	//
	_drop_area.pos.row = event_row - status->offset.row;
	_drop_area.pos.col = event_col - status->offset.col;

	log_debug("pos: %d/%d", _drop_area.pos.row, _drop_area.pos.col);

	//
	// Print the drop area at the new position.
	//
	drop_area_process_blocks(_win_game, &_game_area, &_drop_area, DO_PRINT);
}

/******************************************************************************
 * The function prints the game area, the drop area and the info area centered
 * on the screen. After this function a call to refresh on the game window is
 * necessary.
 *****************************************************************************/

#define DELIM 4

void game_do_center() {

	//
	// Get the sizes of the different areas.
	//
	const s_point game_area_size = s_area_get_size(&_game_area);

	const s_point drop_area_size = s_area_get_size(&_drop_area);

	const s_point info_area_size = info_area_get_size();

	//
	// Compute the size of all areas.
	//
	int total_rows = game_area_size.row;
	int total_cols = game_area_size.col + DELIM + max(info_area_size.col, drop_area_size.col);

	//
	// Get the center positions.
	//
	const int ul_row = (getmaxy(_win_game) - total_rows) / 2;
	const int ul_col = (getmaxx(_win_game) - total_cols) / 2;

	log_debug("upper left row: %d col: %d", ul_row, ul_col);

	s_point_set(&_game_area.pos, ul_row, ul_col);

	s_point_set(&_home, ul_row + info_area_size.row + DELIM, ul_col + game_area_size.col + DELIM);
	s_point_set(&_drop_area.pos, _home.row, _home.col);

	info_area_set_pos(ul_row, _home.col);

	//
	// Delete the old content.
	//
	werase(_win_game);

	//
	// Print the areas at the updated position.
	//
	game_area_print(_win_game, &_game_area);

	drop_area_process_blocks(_win_game, &_game_area, &_drop_area, DO_PRINT);

	info_area_print(_win_game);
}

/******************************************************************************
 * The function initializes the new area.
 *****************************************************************************/

void game_init(s_status *status) {

	//
	// Create the game window
	//
	_win_game = nzc_win_create_fully();

	//
	// Game area
	//
	s_area_create(&_game_area, GAME_SIZE, GAME_SIZE, 2, 4);

	blocks_set(_game_area.blocks, &_game_area.dim, CLR_NONE);

	log_debug("game_area pos: %d/%d", _game_area.pos.row, _game_area.pos.col);

	//
	// The area with the temporary marks
	//
	_marks = blocks_create(_game_area.dim.row, _game_area.dim.col);

	//
	// drop area
	//
	s_area_create(&_drop_area, 3, 3, 2, 4);

	//
	// Fill the blocks with random colors.
	//
	area_update(&_drop_area);

	//
	// Initialize the game status
	//
	s_status_init(status);
}

/******************************************************************************
 * The function frees the allocated memory.
 *****************************************************************************/

void game_free() {

	log_debug_str("Freeing blocks.");

	s_area_free(&_game_area);

	blocks_free(_marks, _game_area.dim.row);

	s_area_free(&_drop_area);

	nzc_win_del(_win_game);
}

/******************************************************************************
 * The function resets the game on the user input.
 *****************************************************************************/

void game_reset(s_status *status) {

	//
	// Initialize the game status
	//
	s_status_init(status);

	//
	// Reset the game area
	//
	s_area_set_blocks(&_game_area, 0);

	//
	// Load a new drop area
	//
	area_update(&_drop_area);

	//
	// Move the drop area to the home position.
	//
	drop_area_move_home(_win_game, &_game_area, &_drop_area, &_home, status);

	//
	// Reset the score
	//
	info_area_reset_score(_win_game);

	//
	// Reset message
	//
	info_area_set_msg(_win_game, "", status);

}

/******************************************************************************
 * The function refreshes the game window.
 *****************************************************************************/

void game_win_refresh() {

	//
	// Move the cursor to a save place and do the refreshing. If the cursor
	// is not moved a flickering can occur. (I am not sure if this is necessary
	// for this game, but I had trouble with it in the past)
	//
	if (wmove(_win_game, 0, 0) == ERR) {
		log_exit_str("Unable to move the cursor!");
	}

	nzc_win_refresh(_win_game);
}

// -------------------------------------------------

/******************************************************************************
 *
 *****************************************************************************/

void game_process_event_home(s_status *status) {

	//
	// Ignore events /
	//
	if (s_status_is_end(status) || !s_status_is_picked_up(status)) {
		return;
	}

	drop_area_process_blocks(_win_game, &_game_area, &_drop_area, DO_DELETE);

	drop_area_move_home(_win_game, &_game_area, &_drop_area, &_home, status);
}

/******************************************************************************
 *
 *****************************************************************************/

void s_area_move_inside(const s_area *outer_area, const s_area *inner_area, s_point *pos) {

	const s_point max_pos = s_area_get_max_inner_pos(outer_area, inner_area);

	log_debug("pos: %d/%d", pos->row, pos->col);

	//
	//
	//
	if (inner_area->pos.row < outer_area->pos.row) {
		pos->row = outer_area->pos.row;

	} else if (inner_area->pos.row >= max_pos.row) {
		pos->row = max_pos.row;
	}

	//
	//
	//
	if (inner_area->pos.col < outer_area->pos.col) {
		pos->col = outer_area->pos.col;

	} else if (inner_area->pos.col >= max_pos.col) {
		pos->col = max_pos.col;
	}

	log_debug("pos: %d/%d", pos->row, pos->col);
}

/******************************************************************************
 *
 *****************************************************************************/

void game_process_event_keyboard(s_status *status, const int diff_row, const int diff_col) {

	s_point event = { .row = _drop_area.pos.row, .col = _drop_area.pos.col };

	log_debug("Drop area: %d/%d diff: %d/%d", event.row, event.col, diff_row, diff_col);

	if (s_area_is_area_inside(&_game_area, &_drop_area)) {

		//
		// The alignment is only necessary if the control switches from mouse
		// to keyboard.
		//
		const bool aligned = s_area_align_point(&_game_area, &event);

		//
		// Move the drop area if possible.
		//
		const bool moved = s_area_move_inner_area(&_game_area, &_drop_area, &event, &(s_point ) { diff_row, diff_col });

		if (aligned || moved) {
			game_process_event_pressed(status, event.row, event.col);
		}

	} else {

		//
		// Move the drop area inside the game area and align its position.
		//
		s_area_move_inside(&_game_area, &_drop_area, &event);
		s_area_align_point(&_game_area, &event);

		game_process_event_pressed(status, event.row, event.col);
	}
}

/******************************************************************************
 *
 *****************************************************************************/

void game_process_event_toggle(s_status *status) {

	if (s_status_is_picked_up(status)) {
		game_process_event_home(status);

	} else {
		game_process_event_pressed(status, _game_area.pos.row, _game_area.pos.col);
	}
}
