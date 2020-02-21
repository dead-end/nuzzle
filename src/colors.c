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
 * The definition of the color pairs..
 *****************************************************************************/

//
// The default color pair is unchanged.
//
#define CP_DEFAULT 0

//
// Used by: attrset(COLOR_PAIR(area->blocks[block.row][block.col]));
//
#define CP_RED_BLACK    1
#define CP_GREEN_BLACK  2
#define CP_BLUE_BLACK   3
#define CP_YELLOW_BLACK 4

//
// Definition of the color pairs for the game background.
//
#define CP_LGR_LGR 5
#define CP_DGR_DGR 6

//
// Define color pairs for all color combinations.
//
#define CP_RED_RED 11
#define CP_RED_GRE 12
#define CP_RED_BLU 13
#define CP_RED_YEL 14

#define CP_GRE_RED 21
#define CP_GRE_GRE 22
#define CP_GRE_BLU 23
#define CP_GRE_YEL 24

#define CP_BLU_RED 31
#define CP_BLU_GRE 32
#define CP_BLU_BLU 33
#define CP_BLU_YEL 34

#define CP_YEL_RED 41
#define CP_YEL_GRE 42
#define CP_YEL_BLU 43
#define CP_YEL_YEL 44

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
 * The function initializes the colors and color pairs. It ensures that the
 * terminal supports colors.
 *****************************************************************************/

void colors_init() {

	//
	// Ensure that the terminal is ok.
	//
	if (!has_colors()) {
		log_exit_str("Terminal does not support colors!");
	}

	if (!can_change_color()) {
		log_exit_str("Terminal does not support color changes!");
	}

	//
	// Start the color.
	//
	if (start_color() != OK) {
		log_exit_str("Unable to init colors!");
	}

	//
	// Initialize colors.
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
	// Initialize color pairs for the chess pattern.
	//
	colors_init_pair(CP_LGR_LGR, BG_L_G, BG_L_G);
	colors_init_pair(CP_DGR_DGR, BG_D_G, BG_D_G);

	//
	// Initialize the color pairs with rgby foreground.
	//
	colors_init_pair(CP_RED_BLACK, FG_RED, COLOR_BLACK);
	colors_init_pair(CP_GREEN_BLACK, FG_GRE, COLOR_BLACK);
	colors_init_pair(CP_BLUE_BLACK, FG_BLU, COLOR_BLACK);
	colors_init_pair(CP_YELLOW_BLACK, FG_YEL, COLOR_BLACK);

	//
	// Initialize the color pairs for the rgby combinations.
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

/******************************************************************************
 * The function fills a block array with random colors. The function ensures
 * that the center block has a color.
 *****************************************************************************/

void colors_init_random(t_block **blocks, const int rows, const int cols) {

	//
	// Get the center block coordinates.
	//
	const int row_center = (rows / 2);
	const int col_center = (cols / 2);

	log_debug("center: %d/%d", row_center, col_center);

	//
	// Set the center color
	//
	blocks[row_center][row_center] = colors_get_color();

	for (int row = 0; row < rows; row++) {
		for (int col = 0; col < cols; col++) {

			//
			// Skip the center block, which is already set.
			//
			if (row == row_center && col == col_center) {
				continue;
			}

			//
			// First check if a block should get a color.
			//
			//if (rand() % 100 < 80) {
			if (rand() % 100 < 75) {
				blocks[row][col] = CLR_NONE;

			} else {
				blocks[row][col] = colors_get_color();
			}

			log_debug("block: %d/%d color: %d", row, col, blocks[row][col]);
		}
	}
}

/******************************************************************************
 * The function sets the color for the info area. It is called with a color of
 * a block. If the color is defined, it is set to the background color of the
 * info area.
 *****************************************************************************/

void colors_info_area_attr(WINDOW *win, const t_block bg_color) {

	switch (bg_color) {

	case CLR_NONE:
		wattrset(win, COLOR_PAIR(CP_DEFAULT));
		break;

	case CLR_RED:
		wattrset(win, A_REVERSE| COLOR_PAIR(CP_RED_BLACK));
		break;

	case CLR_GREEN:
		wattrset(win, A_REVERSE| COLOR_PAIR(CP_GREEN_BLACK));
		break;

	case CLR_BLUE:
		wattrset(win, A_REVERSE| COLOR_PAIR(CP_BLUE_BLACK));
		break;

	case CLR_YELLOW:
		wattrset(win, A_REVERSE| COLOR_PAIR(CP_YELLOW_BLACK));
		break;

	default:
		log_exit("Unknown color: %d", bg_color)
		;
	}
}

/******************************************************************************
 * The function sets the color for the info area if the current game ended.
 *****************************************************************************/

void colors_info_end_attr(WINDOW *win) {
	wattrset(win, A_BLINK| COLOR_PAIR(CP_RED_BLACK));
}

/******************************************************************************
 * The function sets the game color. If foreground and background are not set
 * this is the chess pattern. If only the foreground is defined, we have a
 * simple foreground color and a solid block character. If both are defined we
 * have a foreground and a background color defined and a "transparent" block
 * character.
 *****************************************************************************/

void colors_set_game_attr(WINDOW *win, const t_block fg_color, const t_block bg_color, const bool even) {
	int color_pair;

	//
	// If foreground and background have no color, we use the default
	// background, which is a chess pattern.
	//
	if (bg_color == CLR_NONE && fg_color == CLR_NONE) {
		color_pair = even ? CP_LGR_LGR : CP_DGR_DGR;

	} else {

		if (fg_color == CLR_NONE) {
			color_pair = 10 + bg_color;

		} else if (bg_color == CLR_NONE) {
			color_pair = fg_color * 10 + 1;

		} else {
			color_pair = fg_color * 10 + bg_color;
		}
	}

	log_debug("fg: %d bg: %d pair: %d", fg_color, bg_color, color_pair);

	wattrset(win, COLOR_PAIR(color_pair));
}

/******************************************************************************
 * The function sets the color.
 *****************************************************************************/

void colors_bg_attr(WINDOW *win, const t_block color) {
	wattrset(win, COLOR_PAIR(color));
}

/******************************************************************************
 * The function determines the character to display for a given foreground and
 * background color.
 *****************************************************************************/

wchar_t colors_get_char(const t_block fg_color, const t_block bg_color) {
	wchar_t chr;

	if (fg_color != CLR_NONE) {

		if (bg_color != CLR_NONE) {
			chr = BLOCK_BOTH;

		} else {
			chr = BLOCK_FULL;
		}
	} else {
		chr = BLOCK_EMPTY;
	}

	log_debug("color fg: %d bg: %d char '%lc", fg_color, bg_color, chr);

	return chr;
}
