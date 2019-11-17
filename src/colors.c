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

#include "common.h"
#include "colors.h"

/******************************************************************************
 * The definition of the colors.
 *****************************************************************************/

#define BG_L_G 17
#define BG_D_G 18

#define FG_RED 19
#define BG_RED 20

#define FG_GRE 21
#define BG_GRE 22

#define FG_BLU 23
#define BG_BLU 24

#define FG_YEL 25
#define BG_YEL 26

/******************************************************************************
 * The definition of the color constants.
 *****************************************************************************/

#define FULL 999
#define CORR 100
#define LIGH 600
#define DARK 300

/******************************************************************************
 * Simple wrapper that initializes a color pair with error handling.
 *****************************************************************************/

static void colors_init_pair(const short pair, const short fg, const short bg) {

	if (init_pair(pair, fg, bg) != OK) {
		log_exit("Unable to init color pair: %d fg: %d bg: %d", pair, fg, bg);
	}
}

/******************************************************************************
 * Simple wrapper that initializes a color with error handling.
 *****************************************************************************/

static void colors_init_color(const short color, const short red, const short green, const short blue) {

	if (init_color(color, red, green, blue) != OK) {
		log_exit("Unable to init color: %d red: %d green: %d blue: %d", color, red, green, blue);
	}
}

/******************************************************************************
 *
 *****************************************************************************/

void colors_init() {

	//
	// Ensure that the terminal is ok
	//
	if (!has_colors()) {
		log_exit_str("Terminal does not support colors!");
	}

	if (!can_change_color()) {
		log_exit_str("Terminal does not support color changes!");
	}

	//
	// Start color
	//
	if (start_color() != OK) {
		log_exit_str("Unable to init colors!");
	}

	//
	// Initialize colors
	//
	colors_init_color(BG_L_G, 250, 250, 250);
	colors_init_color(BG_D_G, 230, 230, 230);

	colors_init_color(FG_RED, FULL, DARK, DARK);
	colors_init_color(BG_RED, FULL, LIGH, LIGH);

	colors_init_color(FG_GRE, DARK, FULL - CORR - CORR, DARK);
	colors_init_color(BG_GRE, LIGH, FULL, LIGH);

	colors_init_color(FG_BLU, DARK, DARK, FULL);
	colors_init_color(BG_BLU, LIGH, LIGH, FULL);

	colors_init_color(FG_YEL, FULL - CORR, FULL - CORR, DARK);
	colors_init_color(BG_YEL, FULL, FULL, LIGH);

	//
	// Initialize color pairs
	//
	colors_init_pair(CP_LGR_LGR, BG_L_G, BG_L_G);
	colors_init_pair(CP_DGR_DGR, BG_D_G, BG_D_G);

	colors_init_pair(CP_RED_BLACK, FG_RED, COLOR_BLACK);
	colors_init_pair(CP_GREEN_BLACK, FG_GRE, COLOR_BLACK);
	colors_init_pair(CP_BLUE_BLACK, FG_BLU, COLOR_BLACK);
	colors_init_pair(CP_YELLOW_BLACK, FG_YEL, COLOR_BLACK);

	//
	//
	//
	colors_init_pair(CP_RED_RED, FG_RED, BG_RED);
	colors_init_pair(CP_RED_GRE, FG_RED, BG_GRE);
	colors_init_pair(CP_RED_BLU, FG_RED, BG_BLU);
	colors_init_pair(CP_RED_YEL, FG_RED, BG_YEL);

	colors_init_pair(CP_GRE_RED, FG_GRE, BG_RED);
	colors_init_pair(CP_GRE_GRE, FG_GRE, BG_GRE);
	colors_init_pair(CP_GRE_BLU, FG_GRE, BG_BLU);
	colors_init_pair(CP_GRE_YEL, FG_GRE, BG_YEL);

	colors_init_pair(CP_BLU_RED, FG_BLU, BG_RED);
	colors_init_pair(CP_BLU_GRE, FG_BLU, BG_GRE);
	colors_init_pair(CP_BLU_BLU, FG_BLU, BG_BLU);
	colors_init_pair(CP_BLU_YEL, FG_BLU, BG_YEL);

	colors_init_pair(CP_YEL_RED, FG_YEL, BG_RED);
	colors_init_pair(CP_YEL_GRE, FG_YEL, BG_GRE);
	colors_init_pair(CP_YEL_BLU, FG_YEL, BG_BLU);
	colors_init_pair(CP_YEL_YEL, FG_YEL, BG_YEL);
}

// TODO: Deprecated
short colors_get_pair(const enum e_colors fg, const enum e_colors bg) {
	short result = 0;

	if (fg == color_none) {
		result = 10 + bg;

	} else if (bg == color_none) {
		result = fg * 10 + 1;

	} else {
		result = fg * 10 + bg;
	}

	log_debug("fg: %d bg: %d pair: %d", fg, bg, result);

	return result;
}

/******************************************************************************
 *
 *****************************************************************************/

void colors_info_area_attr(const enum e_colors color) {

	switch (color) {

	case color_none:
		attrset(COLOR_PAIR(CP_DEFAULT));
		break;

	case color_red:
		attrset(A_REVERSE| COLOR_PAIR(CP_RED_BLACK));
		break;

	case color_green:
		attrset(A_REVERSE| COLOR_PAIR(CP_GREEN_BLACK));
		break;

	case color_blue:
		attrset(A_REVERSE| COLOR_PAIR(CP_BLUE_BLACK));
		break;

	case color_yellow:
		attrset(A_REVERSE| COLOR_PAIR(CP_YELLOW_BLACK));
		break;

	default:
		log_exit("Unknown color: %d", color)
		;
	}
}

/******************************************************************************
 *
 *****************************************************************************/

void colors_bg_attr(const enum e_colors color) {
	attrset(COLOR_PAIR(color));
}

//TODO: currently unused
// TODO:
void colors_game_attr(const enum e_colors fg, const enum e_colors bg, const bool even) {
	int color_pair;

	if (bg == color_none && fg == color_none) {
		color_pair = even ? CP_LGR_LGR : CP_DGR_DGR;

	} else {

		if (fg == color_none) {
			color_pair = 10 + bg;

		} else if (bg == color_none) {
			color_pair = fg * 10 + 1;

		} else {
			color_pair = fg * 10 + bg;
		}
	}

	log_debug("fg: %d bg: %d pair: %d", fg, bg, color_pair);

	attrset(COLOR_PAIR(color_pair));
}

/******************************************************************************
 *
 *
 * The function initializes a s_area with random blocks. It is ensured that at
 * least one block is not empty.
 *****************************************************************************/

void colors_init_random2(t_block **blocks, const int rows, const int cols) {
	int count = 0;

	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {

			//
			// First check if the color is set
			//
			if (rand() % 100 < 80) {
				blocks[row][col] = color_none;

			} else {
				blocks[row][col] = (rand() % 4) + 1;
				count++;
			}

			log_debug("row: %d col: %d color: %d", row, col, blocks[row][col]);
		}
	}

	//
	// If all blocks are empty, try again.
	//
	if (count == 0) {
		log_debug_str("Filling failed, try again!");

		colors_init_random(blocks, rows, cols);
	}
}

void colors_init_random(t_block **blocks, const int rows, const int cols) {

	//
	// set center
	//
	const int row_center = (rows / 2);
	const int col_center = (cols / 2);

	log_debug("center: %d/%d", row_center, col_center);

	blocks[row_center][row_center] = colors_get_color();

	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {

			if (row == row_center && col == col_center) {
				continue;
			}

			//
			// First check if the color is set
			//
			if (rand() % 100 < 70) {
				blocks[row][col] = color_none;

			} else {
				blocks[row][col] = colors_get_color();
			}

			log_debug("block: %d/%d color: %d", row, col, blocks[row][col]);
		}
	}
}
