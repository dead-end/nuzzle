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

#include "common.h"
#include <ncurses.h>
#include "s_area.h"
#include "colors.h"

#define GAME_SIZE 11

s_area *game_area;

//TODO: static
static void print_block(const int row, const int col, const int size_row, const int size_col, const wchar_t ch) {

	for (int r = 0; r < size_row; r++) {
		for (int c = 0; c < size_col; c++) {
			mvprintw(row + r, col + c, "%lc", ch);
		}
	}
}

void game_area_print_empty(const s_area *area) {
	s_point pixel;
	s_point block;

	for (block.row = 0; block.row < area->blk_rows; block.row++) {
		for (block.col = 0; block.col < area->blk_cols; block.col++) {

			//
			// Get the absolute position of the upper left pixel of the current
			// block.
			//
			s_area_abs_block(area, &block, &pixel);

			colors_game_attr(color_none, area->blocks[block.row][block.col], (block.row % 2) == (block.col % 2));

			print_block(pixel.row, pixel.col, area->size.row, area->size.col, BLOCK_EMPTY);
		}
	}
}

static short game_get_color_pair(const s_area *matrix, const s_point *pixel, const enum e_colors fg) {

#ifdef DEBUG
	if (pixel->row >= matrix->blk_rows || pixel->col >= matrix->blk_cols) {
		log_exit("Index out of range row: %d col: %d", pixel->row, pixel->col);
	}
#endif

	int bg = matrix->blocks[pixel->row][pixel->col];

	if (bg == color_none && fg == color_none) {
		return (pixel->row % 2) == (pixel->col % 2) ? CP_LGR_LGR : CP_DGR_DGR;
	}

	return colors_get_pair(fg, bg);
}

/******************************************************************************
 *
 *****************************************************************************/

static wchar_t get_char(const s_point *block, const enum e_colors color) {
	wchar_t chr;

	if (color != color_none) {

		if (game_area->blocks[block->row][block->col] != color_none) {
			chr = BLOCK_BOTH;

		} else {
			chr = BLOCK_FULL;
		}
	} else {
		chr = BLOCK_EMPTY;

	}

	log_debug("block: (%d, %d) color: %d char '%lc", block->row, block->col, color, chr);

	return chr;
}

/******************************************************************************
 *
 *****************************************************************************/

void game_area_print_pixel(const s_point *pixel, const enum e_colors color) {
	s_point block;

	log_debug("row: %d col: %d, color: %d", pixel->row, pixel->col, color);

	s_area_get_block(game_area, pixel, &block);

	short color_pair = game_get_color_pair(game_area, &block, color);

	attrset(COLOR_PAIR(color_pair));

	const wchar_t chr = get_char(&block, color);

	mvprintw(pixel->row, pixel->col, "%lc", chr);
}

/******************************************************************************
 *
 *****************************************************************************/

bool game_area_contains(const s_point *pixel) {
	return s_area_contains(game_area, pixel);
}

/******************************************************************************
 *
 *****************************************************************************/

void game_area_free() {
	s_area_free(game_area);
}

/******************************************************************************
 *
 *****************************************************************************/

void game_area_init() {
	log_debug_str("coming soon...");

	game_area = s_area_create(GAME_SIZE, GAME_SIZE);
	s_area_set_abs(game_area, 2, 2);
	s_area_set_size(game_area, 2, 4);

	s_area_init_null(game_area);

	s_area_init_random(game_area);

	game_area_print_empty(game_area);
}
