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
// The 2-dimensional array with temporary data.
//
static t_block **marks;

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

void game_area_get_block(const s_point *pixel, s_point *block) {

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
 * The function prints an individual block. It is called with the upper left
 * corner and the size of the block. It iterate through the block pixels and
 * prints the character.
 *****************************************************************************/

static void print_block(const int ul_row, const int ul_col, const int size_row, const int size_col, const wchar_t ch) {

	const int lr_row = ul_row + size_row;
	const int lr_col = ul_col + size_col;

	for (int row = ul_row; row < lr_row; row++) {
		for (int col = ul_col; col < lr_col; col++) {
			mvprintw(row, col, "%lc", ch);
		}
	}
}

/******************************************************************************
 * The function prints the blocks of the game area. This means empty chars with
 * the background color.
 *****************************************************************************/

void game_area_print() {
	int pixel_row, pixel_col;

	for (int row = 0; row < dim.row; row++) {
		for (int col = 0; col < dim.col; col++) {

			//
			// Get the absolute position of the upper left pixel of the current
			// block.
			//
			pixel_row = block_upper_left(pos.row, size.row, row);
			pixel_col = block_upper_left(pos.col, size.col, col);

			colors_game_attr(color_none, blocks[row][col], (row % 2) == (col % 2));

			print_block(pixel_row, pixel_col, size.row, size.col, BLOCK_EMPTY);
		}
	}
}

/******************************************************************************
 * The function gets the color pair of a pixel (terminal character) of the game
 * area.
 *****************************************************************************/

static short game_get_color_pair(const s_point *pixel, const enum e_colors fg) {

#ifdef DEBUG
	if (pixel->row >= dim.row || pixel->col >= dim.col) {
		log_exit("Index out of range row: %d col: %d", pixel->row, pixel->col);
	}
#endif

	//
	// The game pixel defines the background color.
	//
	int bg = blocks[pixel->row][pixel->col];

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
// TODO:
void game_area_print_pixel(const s_point *pixel, const enum e_colors color) {
	s_point block;

	log_debug("row: %d col: %d, color: %d", pixel->row, pixel->col, color);

	game_area_get_block(pixel, &block);

	const short color_pair = game_get_color_pair(&block, color);

	attrset(COLOR_PAIR(color_pair));

	const wchar_t chr = get_char(&block, color);

	mvprintw(pixel->row, pixel->col, "%lc", chr);
}

/******************************************************************************
 * The functions checks whether a pixel (terminal character) is inside the game
 * area.
 *****************************************************************************/

bool game_area_contains(const int row, const int col) {
	return is_inside_area(&pos, &dim, &size, row, col);
}

/******************************************************************************
 *
 *****************************************************************************/
// TODO:
void game_area_init() {

	s_point_set(&dim, GAME_SIZE, GAME_SIZE);

	blocks = blocks_create(dim.row, dim.col);

	marks = blocks_create(dim.row, dim.col);

	s_point_set(&size, 2, 4);

	blocks_init(blocks, dim.row, dim.col, color_none);

	log_debug("pos: %d/%d", pos.row, pos.col);
}

/******************************************************************************
 * The function frees the allocated memory.
 *****************************************************************************/

void game_area_free() {

	log_debug_str("Freeing blocks.");

	blocks_free(blocks, dim.row);

	blocks_free(marks, dim.row);
}

/******************************************************************************
 * The function returns a struct with the total size of the game area.
 *****************************************************************************/

s_point game_area_get_size() {
	s_point result;

	result.row = dim.row * size.row;
	result.col = dim.col * size.col;

	log_debug("size row: %d col: %d", result.row, result.col);

	return result;
}

/******************************************************************************
 * The function sets the position of the game area. This is done on the
 * initialization and on resizing the terminal.
 *****************************************************************************/

void game_area_set_pos(const int row, const int col) {
	pos.row = row;
	pos.col = col;
}

/******************************************************************************
 * The function checks if the current pixel is aligned with the game area. this
 * does not mean that the pixel is inside the game area.
 *****************************************************************************/

bool game_area_is_aligned(const s_point *pixel) {
	return (pixel->row - pos.row) % size.row == 0 && (pixel->col - pos.col) % size.col == 0;
}

/******************************************************************************
 * The function recursively marks all neighbors, the have the same required
 * color. The recursion stops if all neighbors have different colors or are
 * already marked.
 *****************************************************************************/

void game_area_mark_neighbors(const int row, const int col, t_block color, int *num) {

	//
	// Ensure that we are on the game area.
	//
	if (row < 0 || row >= dim.row || col < 0 || col >= dim.row) {
		log_debug("Outside: %d/%d num: %d color: %d", row, col, *num, color);
		return;
	}

	//
	// Current block has the wrong color.
	//
	if (blocks[row][col] != color) {
		log_debug("Wrong color: %d/%d num: %d color: %d", row, col, *num, color);
		return;
	}

	//
	// Current block is already marked.
	//
	if (marks[row][col] != 0) {
		log_debug("Already marked: %d/%d num: %d color: %d", row, col, *num, color);
		return;
	}

	//
	// Increase the number and mark the block.
	//
	marks[row][col] = ++(*num);

	log_debug("Mark: %d/%d num: %d color: %d", row, col, *num, color);

	//
	// Recursively process the neighbors.
	//
	game_area_mark_neighbors(row + 1, col, color, num);
	game_area_mark_neighbors(row - 1, col, color, num);
	game_area_mark_neighbors(row, col + 1, color, num);
	game_area_mark_neighbors(row, col - 1, color, num);
}

/******************************************************************************
 * The function removes all marked blocks from the game area. As a side effect
 * the marked area is reset.
 *****************************************************************************/

void game_area_remove_marked() {

	for (int row = 0; row < dim.row; row++) {
		for (int col = 0; col < dim.col; col++) {

			if (marks[row][col] > 0) {
				blocks[row][col] = color_none;
				marks[row][col] = 0;
			}
		}
	}
}

/******************************************************************************
 * The function checks whether the current block is empty or not.
 *****************************************************************************/

bool game_area_is_empty(const int row, const int col) {
	return blocks[row][col] == color_none;
}

/******************************************************************************
 * The function resets the marked array.
 *****************************************************************************/

void game_area_reset_marked() {
	blocks_init(marks, dim.row, dim.col, 0);
}

/******************************************************************************
 *
 *****************************************************************************/

void game_area_set_color(const int row, const int col, t_block color) {
	blocks[row][col] = color;
}

