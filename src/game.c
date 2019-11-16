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

#define new_area_fill(a) colors_init_random((a)->blocks, (a)->dim.row, (a)->dim.col);

// ----------------------------------------------------------------------------
#define GAME_SIZE 11

/******************************************************************************
 *
 *****************************************************************************/

//
// The 2-dimensional array with temporary data.
//
static t_block **marks;

/******************************************************************************
 *
 *****************************************************************************/

static void game_area_print(const s_area *game_area) {

	for (int row = 0; row < game_area->dim.row; row++) {
		for (int col = 0; col < game_area->dim.col; col++) {

			colors_game_attr(color_none, game_area->blocks[row][col], (row % 2) == (col % 2));

			s_area_print_block(game_area, row, col, BLOCK_EMPTY);
		}
	}
}

/******************************************************************************
 * The function gets the color pair of a pixel (terminal character) of the game
 * area.
 *****************************************************************************/

static short game_get_color_pair(const s_area *game_area, const s_point *pixel, const enum e_colors fg) {

#ifdef DEBUG
	if (pixel->row >= game_area->dim.row || pixel->col >= game_area->dim.col) {
		log_exit("Index out of range row: %d col: %d", pixel->row, pixel->col);
	}
#endif

	//
	// The game pixel defines the background color.
	//
	int bg = game_area->blocks[pixel->row][pixel->col];

	//
	// If foreground and background have no color, we use the default
	// background.
	//
	if (bg == color_none && fg == color_none) {
		return (pixel->row % 2) == (pixel->col % 2) ? CP_LGR_LGR : CP_DGR_DGR;
	}

	//
	// The concrete color is defined in the colors.c file.
	//
	return colors_get_pair(fg, bg);
}

/******************************************************************************
 * The function determines the character to display for a pixel (terminal
 * character).
 *****************************************************************************/

static wchar_t get_char(const s_area *game_area, const s_point *idx, const enum e_colors fg_color) {
	wchar_t chr;

	if (fg_color != color_none) {

		if (game_area->blocks[idx->row][idx->col] != color_none) {
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
static void game_area_print_pixel(const s_area *game_area, const s_point *pixel, const enum e_colors color) {
	s_point idx;

	log_debug("pixel: %d/%d, color: %d", pixel->row, pixel->col, color);

	s_area_get_block(game_area, pixel, &idx);

	const short color_pair = game_get_color_pair(game_area, &idx, color);

	attrset(COLOR_PAIR(color_pair));

	const wchar_t chr = get_char(game_area, &idx, color);

	mvprintw(pixel->row, pixel->col, "%lc", chr);
}

/******************************************************************************
 *
 *****************************************************************************/
//TODO: name
// TODO: parameter order => ga_idx first or last
int game_area_remove_blocks(s_area *game_area, t_block **drop_blocks, const s_point *ga_idx, const s_point *drop_idx, const s_point *drop_dim) {
	int total = 0;
	int num;

	t_block color;

	for (int row = 0; row < drop_dim->row; row++) {
		for (int col = 0; col < drop_dim->col; col++) {

			color = drop_blocks[drop_idx->row + row][drop_idx->col + col];

			if (color == color_none) {
				log_debug("drop (used) empty: %d/%d", row, col);
				continue;
			}

			num = 0;
			s_area_mark_neighbors(game_area, marks, ga_idx->row + row, ga_idx->col + col, color, &num);
			log_debug("num: %d", num);

			if (num < 4) {
				blocks_set(marks, &game_area->dim, 0);

			} else {
				s_area_remove_marked(game_area, marks);
				total += num;
			}
		}
	}

	return total;
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

	game_print_foreground(&area_pos, &area_size, color_none, BLOCK_EMPTY);
}

/******************************************************************************
 * The function processes all blocks. For each block, the upper left pixel
 * (terminal character) is computed.
 *****************************************************************************/

static void new_area_process_blocks(const bool do_print) {
	s_point na_upper_left;

	for (int row = 0; row < new_area.dim.row; row++) {
		for (int col = 0; col < new_area.dim.col; col++) {

			if (new_area.blocks[row][col] == color_none) {
				continue;
			}

			na_upper_left.row = block_upper_left(new_area.pos.row, new_area.size.row, row);
			na_upper_left.col = block_upper_left(new_area.pos.col, new_area.size.col, col);

			if (do_print) {
				game_print_foreground(&na_upper_left, &new_area.size, new_area.blocks[row][col], BLOCK_FULL);

			} else {
				game_print_foreground(&na_upper_left, &new_area.size, color_none, BLOCK_EMPTY);
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
				s_area_get_offset(new_area, &offset);
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

static bool used_area_is_inside(const s_area *area, const s_point *used_idx, const s_point *used_dim) {

	const int ul_row = area->pos.row + used_idx->row * area->size.row;
	const int ul_col = area->pos.col + used_idx->col * area->size.col;

	if (!s_area_is_inside(&game_area, ul_row, ul_col)) {
		log_debug("used area - upper left not inside: %d/%d", ul_row, ul_col);
		return false;
	}

	const int lr_row = ul_row + (used_dim->row - 1) * area->size.row;
	const int lr_col = ul_col + (used_dim->col - 1) * area->size.col;

	if (!s_area_is_inside(&game_area, lr_row, lr_col)) {
		log_debug("used area - lower right not inside: %d/%d", lr_row, lr_col);
		return false;
	}

	log_debug_str("used area - is inside");

	return true;
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
	s_point used_idx, used_dim;
	blocks_get_used_area(new_area.blocks, &new_area.dim, &used_idx, &used_dim);

	//
	// Ensure that the used area is inside the game area.
	//
	if (!used_area_is_inside(&new_area, &used_idx, &used_dim)) {
		return false;
	}

	//
	// Compute the upper left corner of the used area.
	//
	s_point pixel;
	pixel.row = block_upper_left(new_area.pos.row, new_area.size.row, used_idx.row);
	pixel.col = block_upper_left(new_area.pos.col, new_area.size.col, used_idx.col);

	//
	// Compute the corresponding index in the game area.
	//
	s_area_get_block(&game_area, &pixel, &idx);

	//
	// Check if the used area can be dropped at the game area position.
	//

	// TODO: parameter order => ga_idx first or last
	if (!blocks_drop(game_area.blocks, &idx, new_area.blocks, &used_idx, &used_dim, false)) {
		return false;
	}

	//
	// Drop the used area.
	//
	blocks_drop(game_area.blocks, &idx, new_area.blocks, &used_idx, &used_dim, true);

	//
	// Remove adjacent blocks if possible.
	//
	const int num_removed = game_area_remove_blocks(&game_area, new_area.blocks, &idx, &used_idx, &used_dim);
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

static bool new_area_can_drop() {

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

	blocks_set(game_area.blocks, &game_area.dim, color_none);

	log_debug("game_area pos: %d/%d", game_area.pos.row, game_area.pos.col);

	//
	// Marks
	//
	marks = blocks_create(game_area.dim.row, game_area.dim.col);

	//
	// new area
	//
	s_area_create(&new_area, 3, 3, 2, 4);

	new_area_fill(&new_area);

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
	return blocks_get_size(&game_area.dim, &game_area.size);
}

/******************************************************************************
 * The function returns a struct with the total size of the new area.
 *****************************************************************************/

s_point game_get_new_area_size() {
	return blocks_get_size(&new_area.dim, &new_area.size);
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

		new_area_fill(&new_area);

		if (!new_area_can_drop()) {
			log_debug_str("ENDDDDDDDDDDDDD");
			info_area_set_msg("End");
		}
	} else {
		new_area_delete(&new_area);
	}

	new_area_process(&new_area, HOME_ROW, HOME_COL);
}
