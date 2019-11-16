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
#include "blocks.h"

#include "s_area.h"

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

void game_area_print(const s_area *game_area) {

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

static wchar_t get_char(const s_area *game_area, const s_point *block, const enum e_colors color) {
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
// TODO:
void game_area_print_pixel(const s_area *game_area, const s_point *pixel, const enum e_colors color) {
	s_point block;

	log_debug("pixel: %d/%d, color: %d", pixel->row, pixel->col, color);

	s_area_get_block(game_area, pixel, &block);

	const short color_pair = game_get_color_pair(game_area, &block, color);

	attrset(COLOR_PAIR(color_pair));

	const wchar_t chr = get_char(game_area, &block, color);

	mvprintw(pixel->row, pixel->col, "%lc", chr);
}

/******************************************************************************
 *
 *****************************************************************************/
// TODO:
void game_area_init(s_area *game_area) {

	s_point_set(&game_area->dim, GAME_SIZE, GAME_SIZE);

	game_area->blocks = blocks_create(game_area->dim.row, game_area->dim.col);

	marks = blocks_create(game_area->dim.row, game_area->dim.col);

	s_point_set(&game_area->size, 2, 4);

	blocks_set(game_area->blocks, &game_area->dim, color_none);

	log_debug("pos: %d/%d", game_area->pos.row, game_area->pos.col);
}

/******************************************************************************
 * The function frees the allocated memory.
 *****************************************************************************/

void game_area_free(s_area *game_area) {

	log_debug_str("Freeing blocks.");

	blocks_free(game_area->blocks, game_area->dim.row);

	blocks_free(marks, game_area->dim.row);
}

/******************************************************************************
 * The function recursively marks all neighbors, the have the same required
 * color. The recursion stops if all neighbors have different colors or are
 * already marked.
 *****************************************************************************/

static void game_area_mark_neighbors(const s_area *game_area, const int row, const int col, t_block color, int *num) {

	//
	// Ensure that we are on the game area.
	//
	if (row < 0 || row >= game_area->dim.row || col < 0 || col >= game_area->dim.row) {
		log_debug("Outside: %d/%d num: %d color: %d", row, col, *num, color);
		return;
	}

	//
	// Current block has the wrong color.
	//
	if (game_area->blocks[row][col] != color) {
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
	game_area_mark_neighbors(game_area, row + 1, col, color, num);
	game_area_mark_neighbors(game_area, row - 1, col, color, num);
	game_area_mark_neighbors(game_area, row, col + 1, color, num);
	game_area_mark_neighbors(game_area, row, col - 1, color, num);
}

/******************************************************************************
 *
 *****************************************************************************/
//TODO: name
// TODO: parameter order => ga_idx first or last
int game_area_remove_blocks(const s_area *game_area, t_block **drop_blocks, const s_point *ga_idx, const s_point *drop_idx, const s_point *drop_dim) {
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
			game_area_mark_neighbors(game_area, ga_idx->row + row, ga_idx->col + col, color, &num);
			log_debug("num: %d", num);

			if (num < 4) {
				blocks_set(marks, &game_area->dim, 0);

			} else {
				blocks_remove_marked(game_area->blocks, marks, &game_area->dim);
				total += num;
			}
		}
	}

	return total;
}
