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
#include "home_area.h"
#include "bg_area.h"
#include "rules.h"

 /******************************************************************************
  * Definition of a sleep time. It is used between the dropping of an area and
  * the deletion of blocks. The pause gives the user the chance to see, why the
  * blocks are removed. On debug mode the sleep time is higher.
  *****************************************************************************/

#ifdef DEBUG

#define USLEEP 800000

#else

#define USLEEP 200000

#endif

  /******************************************************************************
   * Definition of the meaning of a flag that make the code more readable.
   *****************************************************************************/

#define DO_PRINT true

#define DO_DELETE false

#define LAYOUT_HORIZONTAL true

#define LAYOUT_VERTICAL false

   /******************************************************************************
	* The number of characters that are used to adjust a block. This means, if an
	* area should be dropped and the drop area is not perfectly aligned with the
	* game area, it is in some cases possible to move the drop area, so that it is
	* aligned after the movement.
	*
	* The definition is the number of columns that can be moved.
	*****************************************************************************/

#define ADJUST 1

	/******************************************************************************
	 * The structs for the game.
	 *****************************************************************************/

	 //
	 // The game area struct
	 //
static s_area _game_area = { .blocks = NULL };

//
// The drop area struct
//
static s_area _drop_area = { .blocks = NULL };

//
// The window used for the game.
//
static WINDOW *_win_game = NULL;

/******************************************************************************
 * The function prints a block of a drop area at a given position, with a given
 * color. The block may overlap areas (game_area, info_area, background area).
 *****************************************************************************/

static void game_print_foreground(WINDOW *win, const s_status *status, const s_area *game_area, const s_point *drop_area_pos, const s_point *drop_area_size, const t_block da_color) {
	s_point pixel;

	//
	// Process the pixels of the block.
	//
	for (pixel.row = drop_area_pos->row; pixel.row < drop_area_pos->row + drop_area_size->row; pixel.row++) {
		for (pixel.col = drop_area_pos->col; pixel.col < drop_area_pos->col + drop_area_size->col; pixel.col++) {

			//
			// Check the position of each block pixel.
			//
			if (s_area_is_inside(game_area, &pixel)) {
				s_area_print_chess_pixel(win, game_area, &pixel, da_color, status->game_cfg->chess_type);

			} else if (info_area_contains(&pixel)) {
				info_area_print_pixel(win, &pixel, da_color);

			} else if (home_area_get_idx(&pixel) >= 0) {
				home_area_print_pixel(win, status, &pixel, da_color);

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

static void drop_area_process_blocks(WINDOW *win, const s_status *status, const s_area *game_area, const s_area *drop_area, const bool do_print) {
	s_point drop_area_ul;
	s_point idx;

	log_debug("drop area pos: %d/%d", drop_area->pos.row, drop_area->pos.col);

	for (idx.row = 0; idx.row < drop_area->dim.row; idx.row++) {
		for (idx.col = 0; idx.col < drop_area->dim.col; idx.col++) {

			//
			// If the drop area block is empty, there is nothing to do.
			//
			if (drop_area->blocks[idx.row][idx.col] == CLR_NONE) {
				continue;
			}

			//
			// Compute the upper left corner of the current block of the drop
			// area.
			//
			drop_area_ul = s_area_get_ul(drop_area, &idx);

			if (do_print) {
				game_print_foreground(win, status, game_area, &drop_area_ul, &drop_area->size, drop_area->blocks[idx.row][idx.col]);

			} else {
				game_print_foreground(win, status, game_area, &drop_area_ul, &drop_area->size, CLR_NONE);
			}
		}
	}

	log_debug_str("end");
}

/******************************************************************************
 * The function tries the adjust the drop area, which means moving it one
 * character to the left or right.
 *****************************************************************************/

static bool do_adjust(const s_area *game_area, const s_area *drop_area, s_area *adj_area) {

	//
	// If the drop area is aligned, there is nothing to do.
	//
	if (s_area_is_aligned(game_area, drop_area->pos.row, drop_area->pos.col)) {
		return true;
	}

	//
	// If the moved drop area is aligned, we adjust the position and return.
	//
	if (s_area_is_aligned(game_area, drop_area->pos.row, drop_area->pos.col + ADJUST)) {
		adj_area->pos.col += ADJUST;
		return true;
	}

	//
	// If the moved drop area is aligned, we adjust the position and return.
	//
	if (s_area_is_aligned(game_area, drop_area->pos.row, drop_area->pos.col - ADJUST)) {
		adj_area->pos.col -= ADJUST;
		return true;
	}

	log_debug_str("New blocks are not aligned!");
	return false;
}

/******************************************************************************
 * The function moves the drop area to a given position.
 *****************************************************************************/

static void animate_move(WINDOW *win, const s_status *status, s_area *game_area, s_area *drop_area, const s_point *to) {

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
	drop_area_process_blocks(win, status, game_area, &_drop_area, DO_DELETE);

	//
	// Move the drop area to the new position.
	//
	s_point_copy(&drop_area->pos, to);

	//
	// Print and show the drop area at the new position.
	//
	drop_area_process_blocks(win, status, game_area, &_drop_area, DO_PRINT);

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

static void animate_drop(WINDOW *win, const s_status *status, s_area *game_area, const s_point *drop_point, const s_area *drop_area) {

	log_debug("Dropping drop area: %d/%d at game: %d/%d", drop_area->pos.row, drop_area->pos.col, drop_point->row, drop_point->col);

	//
	// Drop the used area.
	//
	s_area_drop(game_area, drop_point, drop_area, true);

	//
	// Delete the drop area from the foreground
	//
	drop_area_process_blocks(win, status, &_game_area, &_drop_area, DO_DELETE);

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

static bool game_area_can_drop(s_area *game_area, s_point *drop_point, const s_area *drop_area, s_area *adj_area) {

	//
	// Copy the drop area to the adjusted area.
	//
	s_area_copy(drop_area, adj_area);

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

/******************************************************************************
 * The function computes the layout of the game. The home area is printed
 * horizontal under the info area and both right to the game area.
 *****************************************************************************/

static void layout_horizontal(const s_point *win_size, const s_point *game_area_size, const s_point *info_area_size, const s_point *delim) {

	//
	// Compute the size of the home area.
	//
	const s_point home_area_size = home_area_get_size(LAYOUT_HORIZONTAL);

	//
	// Compute the size of all areas.
	//
	const int total_rows = game_area_size->row;
	const int total_cols = game_area_size->col + delim->col + max(home_area_size.col, info_area_size->col);

	//
	// Get the center positions.
	//
	const int ul_row = (win_size->row - total_rows) / 2;
	const int ul_col = (win_size->col - total_cols) / 2;

	log_debug("upper left row: %d col: %d", ul_row, ul_col);

	//
	// Set the position of the game area, which is the upper left corner.
	//
	s_point_set(&_game_area.pos, ul_row, ul_col);

	//
	// Set the position of the info area, which is top right to the game area.
	//
	info_area_set_pos(ul_row, ul_col + game_area_size->col + delim->col);

	//
	// Set the position of the home area, which is right to the game area and
	// under the info area.
	//
	const s_point home_pos = { ul_row + info_area_size->row + delim->row, _game_area.pos.col + game_area_size->col + delim->col };
	home_area_layout(&home_pos, LAYOUT_HORIZONTAL);
}

/******************************************************************************
 * The function moves the drop area to the first possible position on the game
 * area.
 *****************************************************************************/

static inline void game_mv_possible_pos(s_status *status) {
	s_point idx;

	//
	// Get the index of the first position where the drop area can be dropped.
	//
	if (!s_area_can_drop_anywhere(&_game_area, &_drop_area, &idx)) {
		log_exit_str("Unexpected end!");
	}

	//
	// Get the absolute upper left position of the index.
	//
	const s_point pos = s_area_get_ul(&_game_area, &idx);

	log_debug("Possible - idx: %d/%d - pos: %d/%d", idx.row, idx.col, pos.row, pos.col);

	game_event_move(status, &pos);
}

// ----------------------------------------
// INTERFACE

/******************************************************************************
 * The function creates and initializes all data structures for a new game of
 * a given type. The s_game_cfg struct contains the definition of the selected
 * game.
 *****************************************************************************/

void game_create_game(const s_status *status) {

	//
	// Save the game configuration to a variable for easier access.
	//
	const s_game_cfg *game_cfg = status->game_cfg;
	log_debug("Create game: %s", game_cfg->title);

	//
	// Create and initialize the game area.
	//
	s_area_create(&_game_area, &game_cfg->game_dim, &game_cfg->game_size);

	blocks_set(_game_area.blocks, &_game_area.dim, CLR_NONE);

	log_debug("game_area pos: %d/%d", _game_area.pos.row, _game_area.pos.col);

	//
	// Create the drop area
	//
	s_area_create(&_drop_area, &status->game_cfg->drop_dim, &game_cfg->game_size);

	//
	// Create and initialize the rules for the game.
	//
	rules_create_game(&_game_area);

	//
	// Set / load game data
	//
	game_cfg->fct_ptr_set_data(status->game_cfg->data);

	//
	// Create and initialize thehome area
	//
	home_area_create_game(status->game_cfg);

	//
	// Initialize the info area
	//
	info_area_init(status);
}

/******************************************************************************
 * The function frees the memory necessary for a game. It is possible that the
 * game does not started.
 *****************************************************************************/

void game_free_game(const s_status *status) {

	log_debug("Freeing game area: %d/%d", _game_area.dim.row, _game_area.dim.col);

	s_area_free(&_game_area);

	rules_free_game(&_game_area);

	//
	// Without game configuration, we cannot free the drop area.
	//
	if (status->game_cfg != NULL) {

		log_debug("Freeing drop area: %d/%d", _drop_area.dim.row, _drop_area.dim.col);

		//
		// The normalization of the drop area changes the dimension. To be able
		// to free the drop area, we have to reset the dimension.
		//
		s_point_set(&_drop_area.dim, status->game_cfg->drop_dim.row, status->game_cfg->drop_dim.col);

		s_area_free(&_drop_area);
	}

	home_area_free_game();
}

/******************************************************************************
 * The function processes a new "released" event. This requires checking if the
 * drop area can be dropped, and if so, if the game ended.
 *****************************************************************************/

bool game_event_drop(s_status *status) {

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
		animate_move(_win_game, status, &_game_area, &_drop_area, &adj_area.pos);

		//
		// Drop the drop area.
		//
		animate_drop(_win_game, status, &_game_area, &drop_point, &_drop_area);

		const int num_removed = status->game_cfg->fct_ptr_rules_remove(&_game_area);

		if (num_removed > 0) {
			info_area_update_score_turns(_win_game, status, num_removed);
			s_area_print_chess(_win_game, &_game_area, status->game_cfg->chess_type);

		} else {
			info_area_new_turn(_win_game, status);
		}

		//
		// Dropping the drop area means copying the blocks to the background.
		// After this, the drop area can be deleted from the foreground.
		//
		drop_area_process_blocks(_win_game, status, &_game_area, &_drop_area, DO_DELETE);

		//
		// Mark the home area as dropped. This also means not picked up.
		//
		home_area_mark_drop();

		//
		// Mark the drop area as not picked up.
		//
		s_status_undo_pickup(status);

		if (home_area_refill(status->game_cfg, false)) {
			home_area_print(_win_game, status);
		}

		//
		// Check if we can drop one of the home areas. If not the game is
		// finished.
		//
		if (!home_area_can_drop_anywhere(&_game_area)) {

			//
			// If there is no place to drop it, we finished.
			//
			s_status_set_end(status);
			info_area_set_end(_win_game, status);
			log_debug_str("ENDDDDDDDDDDDDD");
		}

		return true;
	}

	return false;
}

/******************************************************************************
 * The function prints the game area, the drop area and the info area centered
 * on the screen. After this function a call to refresh on the game window is
 * necessary.
 *****************************************************************************/

void game_do_center(const s_status *status) {

	const s_point game_area_size = s_area_get_size(&_game_area);

	const s_point info_area_size = info_area_get_size();

	const s_point win_size = { getmaxy(_win_game), getmaxx(_win_game) };

	layout_horizontal(&win_size, &game_area_size, &info_area_size, &status->game_cfg->game_size);

	//
	// Delete the old content.
	//
	werase(_win_game);

	//
	// Print the areas at the updated position.
	//
	s_area_print_chess(_win_game, &_game_area, status->game_cfg->chess_type);

	home_area_print(_win_game, status);

	//
	// If the drop area is picked up, we need to print it.
	//
	if (s_status_is_picked_up(status)) {
		drop_area_process_blocks(_win_game, status, &_game_area, &_drop_area, DO_PRINT);
	}

	info_area_print(_win_game, status);
}

/******************************************************************************
 * The function initializes the new area.
 *****************************************************************************/

void game_init() {

	//
	// Create the game window
	//
	_win_game = nzc_win_create_fully();

	//
	// Set the default color pairs
	//
	wbkgd(_win_game, color_default_bg());

	wbkgd(stdscr, color_default_bg());

	//
	// Refresh the game window to show the default color pairs.
	//
	nzc_win_refresh(stdscr);
}

/******************************************************************************
 * The function frees the allocated memory.
 *****************************************************************************/

void game_free() {

	nzc_win_del(_win_game);
}

/******************************************************************************
 * The function resets the game on the user input.
 *****************************************************************************/

void game_reset(s_status *status) {

	//
	// Initialize the game status
	//
	s_status_init(status, status->game_cfg);

	//
	// Reset the game area
	//
	s_area_set_blocks(&_game_area, 0);

	//
	// Move the drop area to the home position.
	//
	home_area_reset(status->game_cfg);
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

// ------------------------- events

/******************************************************************************
 * The function moves the drop area with a given event. It is required that the
 * drop area is picked up.
 *****************************************************************************/

void game_event_move(s_status *status, const s_point *event) {

	log_debug("Event pos: %d/%d", event->row, event->col);

#ifdef DEBUG

	//
	// Ensure that the drop area is picked up.
	//
	if (!s_status_is_picked_up(status)) {
		log_exit_str("Drop area not picked up!");
	}
#endif

	//
	// If the position did not changed, we do nothing.
	//
	if (s_area_same_pos(&_drop_area, event->row, event->col)) {
		return;
	}

	//
	// Delete the drop area at its old position.
	//
	drop_area_process_blocks(_win_game, status, &_game_area, &_drop_area, DO_DELETE);

	//
	// Set the new position of the drop area.
	//
	s_status_update_pos(status, &_drop_area.pos, event);

	//
	// Print the drop area at the new position.
	//
	drop_area_process_blocks(_win_game, status, &_game_area, &_drop_area, DO_PRINT);
}

/******************************************************************************
 * The function picks up the drop area. It is required that the drop area is
 * not already picked up.
 *****************************************************************************/

void game_process_do_pickup(s_status *status, const s_point *event) {

	log_debug("Event pos: %d/%d", event->row, event->col);

#ifdef DEBUG

	//
	// Ensure that the drop area is not already picked up.
	//
	if (s_status_is_picked_up(status)) {
		log_exit_str("Already picked up!");
	}
#endif

	//
	// If we can pickup something, we mark the home area as picked up
	// and print the empty home area.
	//
	if (!home_area_pickup(&_drop_area, event)) {
		return;
	}
	home_area_print(_win_game, status);

	//
	// If the event is inside the home area we compute the exact position.
	//
	if (s_area_is_inside(&_drop_area, event)) {
		s_status_pickup(status, event->row - _drop_area.pos.row, event->col - _drop_area.pos.col);
	} else {
		s_status_pickup(status, 0, 0);
	}

	//
	// Set the new position of the drop area.
	//
	s_status_update_pos(status, &_drop_area.pos, event);

	//
	// Print the drop area at the new position.
	//
	drop_area_process_blocks(_win_game, status, &_game_area, &_drop_area, DO_PRINT);
}

/******************************************************************************
 * The function is called with a picked up area. The area is released and moved
 * to its home position. It is required that the drop area is picked up.
 *****************************************************************************/

void game_process_event_undo_pickup(s_status *status) {

	//
	// Delete the drop area from the old position.
	//
	drop_area_process_blocks(_win_game, status, &_game_area, &_drop_area, DO_DELETE);

	//
	// Mark the status as not picked up
	//
	s_status_undo_pickup(status);

	//
	// Mark the home area as not picked up.
	//
	home_area_undo_pickup();
	home_area_print(_win_game, status);
}

/******************************************************************************
 * The function processes a keyboard event. It translates the key to a position
 * of the drop area. It is possible that the drop area is not on the game area.
 *****************************************************************************/

void game_event_keyboard_mv(s_status *status, const int diff_row, const int diff_col) {

	log_debug("Drop area: %d/%d diff: %d/%d", _drop_area.pos.row, _drop_area.pos.col, diff_row, diff_col);

	//
	// We cannot move an area which is not picked up.
	//
	if (!s_status_is_picked_up(status)) {
		return;
	}

	//
	// Mark as keyboard event.
	//
	s_status_keyboard_event(status);

	if (s_area_is_area_inside(&_game_area, &_drop_area)) {

		s_point event = { .row = _drop_area.pos.row, .col = _drop_area.pos.col };

		//
		// The alignment is only necessary if the control switches from mouse
		// to keyboard.
		//
		// TODO: the function s_area_move_inner_area() assumes that the event
		// point is the position of the drop area.
		const bool aligned = s_area_align_point(&_game_area, &event);

		//
		// Move the drop area if possible.
		//
		const bool moved = s_area_move_inner_area(&_game_area, &_drop_area, &event, &(s_point) { diff_row, diff_col });

		if (aligned || moved) {
			game_event_move(status, &event);
		}

	} else {

		//
		// Move the drop area to the first possible position.
		//
		game_mv_possible_pos(status);
	}
}

/******************************************************************************
 * The function picks up the next unused home area.
 *****************************************************************************/

void game_event_next_home_area(s_status *status) {

	//
	// Mark as keyboard event.
	//
	s_status_keyboard_event(status);

	//
	// If a home area is picked up, we are looking for the next unused.
	//
	if (s_status_is_picked_up(status)) {
		s_point unused;

		//
		// Get the next unused home area. If one one unused home area was left,
		// the function returns false.
		//
		const bool found = home_area_next_unused(&unused);

		//
		// If an new unused home area was found, we switch to this home area.
		//
		if (found) {
			game_process_event_undo_pickup(status);

			game_process_do_pickup(status, &unused);
		}

	}

	//
	// At this point no home area is picked up, so we get the first possible.
	//
	else {
		s_point unused;

		if (!home_area_next_unused(&unused)) {
			log_exit_str("No unused found!");
		}

		game_process_do_pickup(status, &unused);
	}
}
