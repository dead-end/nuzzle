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
static s_point home;

//
// The offset is the difference between the position of a mouse event and the
// upper left corner. For the printing we need the upper left corner.
//
static s_point offset;

#define OFFSET_NOT_SET -1

#define ADJUST 1

static s_area new_area;

static s_area game_area;

#define GAME_SIZE 11

/******************************************************************************
 *
 *****************************************************************************/

//
// The 2-dimensional array with temporary data.
//
static t_block **marks;

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
	s_point idx;

	log_debug("pixel: %d/%d, color: %d", pixel->row, pixel->col, fg_color);

	s_area_get_block(game_area, pixel, &idx);

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

static void game_print_foreground(const s_point *area_pos, const s_point *area_size, const t_block color, const wchar_t chr) {
	s_point pixel;

	for (pixel.row = area_pos->row; pixel.row < area_pos->row + area_size->row; pixel.row++) {
		for (pixel.col = area_pos->col; pixel.col < area_pos->col + area_size->col; pixel.col++) {

			if (s_area_is_inside(&game_area, pixel.row, pixel.col)) {
				game_area_print_pixel(&game_area, &pixel, color);

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

static void new_area_delete(const s_area *new_area) {
	s_point area_pos = { .row = new_area->pos.row, .col = new_area->pos.col - ADJUST };
	s_point area_size = { .row = new_area->size.row * new_area->dim.row, .col = new_area->size.col * new_area->dim.col + 2 * ADJUST };

	game_print_foreground(&area_pos, &area_size, CLR_NONE, BLOCK_EMPTY);
}

/******************************************************************************
 * The function processes all blocks. For each block, the upper left pixel
 * (terminal character) is computed.
 *****************************************************************************/

static void new_area_process_blocks(const bool do_print) {
	s_point na_upper_left;

	for (int row = 0; row < new_area.dim.row; row++) {
		for (int col = 0; col < new_area.dim.col; col++) {

			if (new_area.blocks[row][col] == CLR_NONE) {
				continue;
			}

			na_upper_left.row = block_upper_left(new_area.pos.row, new_area.size.row, row);
			na_upper_left.col = block_upper_left(new_area.pos.col, new_area.size.col, col);

			if (do_print) {
				game_print_foreground(&na_upper_left, &new_area.size, new_area.blocks[row][col], BLOCK_FULL);

			} else {
				game_print_foreground(&na_upper_left, &new_area.size, CLR_NONE, BLOCK_EMPTY);
			}
		}
	}
}

/******************************************************************************
 * The function processes a new mouse event.
 *****************************************************************************/

static void new_area_process(s_area *new_area, const int event_row, const int event_col) {

	//
	// If the row or col is negative, we move to the home position.
	//
	if (event_row == HOME_ROW || event_col == HOME_COL) {

		s_point_set(&new_area->pos, home.row, home.col);

		s_point_set(&offset, OFFSET_NOT_SET, OFFSET_NOT_SET);

	} else {

		if (offset.row == OFFSET_NOT_SET && offset.col == OFFSET_NOT_SET) {
			if (s_area_is_inside(new_area, event_row, event_col)) {
				offset.row = event_row - home.row;
				offset.col = event_col - home.col;

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
				s_point_set(&offset, used_area.idx.row * new_area->size.row, used_area.idx.col * new_area->size.col);
			}
		}

		new_area->pos.row = event_row - offset.row;
		new_area->pos.col = event_col - offset.col;

		log_debug("pos: %d/%d offset:  %d/%d", new_area->pos.row, new_area->pos.col, offset.row, offset.col);
	}

	new_area_process_blocks(DO_PRINT);
}

/******************************************************************************
 *
 *****************************************************************************/

static bool new_area_is_dropped() {
	s_point idx;

	//
	// Ensure that the blocks are aligned.
	//
	if (!s_area_is_aligned(&game_area, new_area.pos.row, new_area.pos.col)) {

		if (s_area_is_aligned(&game_area, new_area.pos.row, new_area.pos.col + ADJUST)) {
			new_area.pos.col += ADJUST;

		} else if (s_area_is_aligned(&game_area, new_area.pos.row, new_area.pos.col - ADJUST)) {
			new_area.pos.col -= ADJUST;

		} else {
			log_debug_str("New blocks are not aligned!");
			return false;
		}
	}

	//
	// Compute the used area of the new blocks.
	//
	s_used_area used_area;
	s_area_get_used_area(&new_area, &used_area);

	//
	// Ensure that the used area is inside the game area.
	//
	if (!s_area_used_area_is_inside(&game_area, &used_area)) {
		return false;
	}

	//
	// Compute the upper left corner of the used area.
	//
	// TODO: function to compute the upper left corner of the used area, which has a reference to the underlying area.
	// TODO: s_area_used_upper_left
	s_point pixel;
	pixel.row = block_upper_left(new_area.pos.row, new_area.size.row, used_area.idx.row);
	pixel.col = block_upper_left(new_area.pos.col, new_area.size.col, used_area.idx.col);

	//
	// Compute the corresponding index in the game area.
	//
	s_area_get_block(&game_area, &pixel, &idx);

	//
	// Check if the used area can be dropped at the game area position.
	//
	if (!s_area_drop(&game_area, &idx, &used_area, false)) {
		return false;
	}

	//
	// Drop the used area.
	//
	s_area_drop(&game_area, &idx, &used_area, true);

	//
	// Remove adjacent blocks if possible.
	//
	const int num_removed = s_area_remove_blocks(&game_area, &idx, &used_area, marks);
	if (num_removed >= 4) {
		info_area_add_to_score(num_removed);
		game_area_print(&game_area);
	}

	log_debug_str("Is dropped!");

	return true;
}

/******************************************************************************
 *
 *****************************************************************************/

static bool new_area_can_drop_anywhere() {

	s_used_area used_area;

	s_area_get_used_area(&new_area, &used_area);

	return s_area_can_drop_anywhere(&game_area, &used_area);
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
	s_area_create(&game_area, GAME_SIZE, GAME_SIZE, 2, 4);

	blocks_set(game_area.blocks, &game_area.dim, CLR_NONE);

	log_debug("game_area pos: %d/%d", game_area.pos.row, game_area.pos.col);

	//
	// Marks
	//
	marks = blocks_create(game_area.dim.row, game_area.dim.col);

	//
	// new area
	//
	s_area_create(&new_area, 3, 3, 2, 4);

	//
	// Fill the blocks with random colors.
	//
	colors_init_random(new_area.blocks, new_area.dim.row, new_area.dim.col);

	//
	// rest
	//
	s_point_set(&offset, -1, -1);
}

/******************************************************************************
 * The function frees the allocated memory.
 *****************************************************************************/

void game_free() {

	log_debug_str("Freeing blocks.");

	s_area_free(&game_area);

	blocks_free(marks, game_area.dim.row);

	s_area_free(&new_area);
}

/******************************************************************************
 *
 *****************************************************************************/

s_point game_get_game_area_size() {
	return s_area_get_size(&game_area);
}

/******************************************************************************
 * The function returns a struct with the total size of the new area.
 *****************************************************************************/

s_point game_get_new_area_size() {
	return s_area_get_size(&new_area);
}

/******************************************************************************
 *
 *****************************************************************************/

void game_set_game_area_pos(const int row, const int col) {
	s_point_set(&game_area.pos, row, col);
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
	s_point_set(&home, row, col);

	//
	// The initial position is the home position. It can change by the mouse
	// motion.
	//
	s_point_set(&new_area.pos, row, col);

	log_debug("position: %d/%d", new_area.pos.row, new_area.pos.col);
}

/******************************************************************************
 *
 *****************************************************************************/

void game_print_game_area() {
	game_area_print(&game_area);
}

/******************************************************************************
 *
 *****************************************************************************/

void game_print_new_area() {
	new_area_process_blocks(DO_PRINT);
}

/******************************************************************************
 *
 *****************************************************************************/

void game_process_event_pressed(const int row, const int col) {

	if (!s_area_same_pos(&new_area, row, col)) {

		new_area_process_blocks(DO_DELETE);

		new_area_process(&new_area, row, col);
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
	if (new_area_is_dropped()) {

		//
		// Delete the current none-empty blocks. After calling
		// new_area_fill(), the non-empty blocks may be different.
		//
		new_area_delete(&new_area);

		//
		// Fill the blocks with new, random colors.
		//
		colors_init_random(new_area.blocks, new_area.dim.row, new_area.dim.col);

		if (!new_area_can_drop_anywhere()) {
			log_debug_str("ENDDDDDDDDDDDDD");
			info_area_set_msg("End");
		}
	} else {
		new_area_delete(&new_area);
	}

	new_area_process(&new_area, HOME_ROW, HOME_COL);
}
