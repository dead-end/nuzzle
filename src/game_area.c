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

#include <blocks.h>
#include <ncurses.h>

#include "colors.h"
#include "common.h"

#define GAME_SIZE 11

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

/******************************************************************************
 * The function gets the block for an absolute pixel.
 *****************************************************************************/

static void game_area_get_block(const s_point *pixel, s_point *block) {

#ifdef DEBUG
	if (size.row == 0 || size.col == 0) {
		log_exit_str("Size not set!");
	}
#endif

	block->row = (pixel->row - pos.row) / size.row;
	block->col = (pixel->col - pos.col) / size.col;

	log_debug("pixel - row: %d col: %d block - row: %d col: %d", pixel->row, pixel->col, block->row, block->col);
}

/******************************************************************************
 *
 *****************************************************************************/

//TODO: static
static void print_block(const int row, const int col, const int size_row, const int size_col, const wchar_t ch) {

	for (int r = 0; r < size_row; r++) {
		for (int c = 0; c < size_col; c++) {
			mvprintw(row + r, col + c, "%lc", ch);
		}
	}
}

/******************************************************************************
 *
 *****************************************************************************/

//#define block_upper_left(pos,size,idx) (pos) + (size) * (idx)
static void game_area_print_empty() {
	int pixel_row, pixel_col;

	for (int row = 0; row < dim.row; row++) {
		for (int col = 0; col < dim.col; col++) {

			//
			// Get the absolute position of the upper left pixel of the current
			// block.
			//
			pixel_row = block_upper_left(pos.row, size.row, row);
			pixel_col = block_upper_left(pos.col, size.col, col);

			//colors_game_attr(color_none, game_area->blocks[row][col], (row % 2) == (col % 2));
			colors_game_attr(color_none, blocks[row][col], (row % 2) == (col % 2));

			print_block(pixel_row, pixel_col, size.row, size.col, BLOCK_EMPTY);
		}
	}
}

/******************************************************************************
 *
 *****************************************************************************/

static short game_get_color_pair(const s_point *pixel, const enum e_colors fg) {

#ifdef DEBUG
	if (pixel->row >= dim.row || pixel->col >= dim.col) {
		log_exit("Index out of range row: %d col: %d", pixel->row, pixel->col);
	}
#endif

	int bg = blocks[pixel->row][pixel->col];

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

		if (blocks[block->row][block->col] != color_none) {
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

	log_debug("pos: %d/%d", pos.row, pos.col);
	game_area_get_block(pixel, &block);
	log_debug("pos: %d/%d", pos.row, pos.col);

	short color_pair = game_get_color_pair(&block, color);

	attrset(COLOR_PAIR(color_pair));

	const wchar_t chr = get_char(&block, color);

	mvprintw(pixel->row, pixel->col, "%lc", chr);
}

/******************************************************************************
 *
 *****************************************************************************/

bool game_area_contains(const s_point *pixel) {

	log_debug("pos: %d/%d", pos.row, pos.col);

	if (pixel->row < pos.row || pixel->col < pos.col) {
		return false;
	}

	if (pixel->row >= pos.row + dim.row * size.row || pixel->col >= pos.col + dim.col * size.col) {
		return false;
	}

	return true;
}

/******************************************************************************
 *
 *****************************************************************************/

void game_area_free() {
	blocks_free(blocks, dim.row);
}

/******************************************************************************
 *
 *****************************************************************************/

void game_area_init() {
	log_debug_str("coming soon...");

	s_point_set(&dim, GAME_SIZE, GAME_SIZE);

	blocks = blocks_create(dim.row, dim.col);

	s_point_set(&pos, 2, 2);
	s_point_set(&size, 2, 4);

	blocks_init(blocks, dim.row, dim.col, color_none);

	colors_init_random(blocks, dim.row, dim.col);

	game_area_print_empty();

	log_debug("pos: %d/%d", pos.row, pos.col);
}
