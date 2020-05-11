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
 * The definition of the colors, that are used inside this game. Additionally
 * COLOR_BLACK is used.
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

#define CP_START 8

//
// The number of colors for the foreground and the background.
//
#define NUM_COLORS 7

#define COLOR_PAIR_UNDEF -1

static t_block _color_pairs[NUM_COLORS][NUM_COLORS];

/******************************************************************************
 * The function initializes the array with the color pairs. They are set to an
 * undefined value. Not all combinations of color pairs are necessary.
 *****************************************************************************/

static void color_pairs_init() {

	for (int fg = 0; fg < NUM_COLORS; fg++) {
		for (int bg = 0; bg < NUM_COLORS; bg++) {
			_color_pairs[fg][bg] = COLOR_PAIR_UNDEF;
		}
	}
}

/******************************************************************************
 * The function returns a color pair for a pair of colors. It simply returns
 * the value of the _color_pairs array. Because not all color pair combinations
 * are defined, we check if the combination is valid in debug mode.
 *****************************************************************************/

static inline t_block color_pair_get(const short fg, const short bg) {
	const t_block cp = _color_pairs[fg][bg];

#ifdef DEBUG
	log_debug("Color pair: %d fg: %d bg: %d", cp, fg, bg);

	if (cp == COLOR_PAIR_UNDEF) {
		log_exit("Color pair is not defined - fg: %d bg: %d", fg, bg);
	}
#endif

	return cp;
}

/******************************************************************************
 * Simple wrapper that initializes a color pair with error handling.
 *****************************************************************************/

static short colors_init_pair(const short pair, const short fg, const short bg) {

	if (init_pair(pair, fg, bg) != OK) {
		log_exit("Unable to init color pair: %d fg: %d bg: %d", pair, fg, bg);
	}

	return pair;
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
 * initialize colors and color pairs.
 *****************************************************************************/

static void colors_init_colors() {

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
	// We do not have explicit color pairs.
	//
	short color_pair = CP_START;

	_color_pairs[CLR_NONE][CLR_NONE] = CP_DEFAULT;

	//
	// Initialize color pairs for the chess pattern.
	//
	_color_pairs[CLR_NONE][CLR_GREY_LIGHT] = colors_init_pair(color_pair++, BG_L_G, BG_L_G);
	_color_pairs[CLR_NONE][CLR_GREY_DARK] = colors_init_pair(color_pair++, BG_D_G, BG_D_G);

	//
	// Initialize the color pairs with rgby foreground.
	//
	_color_pairs[CLR_RED][CLR_NONE] = colors_init_pair(color_pair++, FG_RED, COLOR_BLACK);
	_color_pairs[CLR_GREEN][CLR_NONE] = colors_init_pair(color_pair++, FG_GRE, COLOR_BLACK);
	_color_pairs[CLR_BLUE][CLR_NONE] = colors_init_pair(color_pair++, FG_BLU, COLOR_BLACK);
	_color_pairs[CLR_YELLOW][CLR_NONE] = colors_init_pair(color_pair++, FG_YEL, COLOR_BLACK);

	_color_pairs[CLR_NONE][CLR_RED] = colors_init_pair(color_pair++, COLOR_BLACK, BG_RED);
	_color_pairs[CLR_NONE][CLR_GREEN] = colors_init_pair(color_pair++, COLOR_BLACK, BG_GRE);
	_color_pairs[CLR_NONE][CLR_BLUE] = colors_init_pair(color_pair++, COLOR_BLACK, BG_BLU);
	_color_pairs[CLR_NONE][CLR_YELLOW] = colors_init_pair(color_pair++, COLOR_BLACK, BG_YEL);

	//
	// Initialize the color pairs for the rgby combinations.
	//
	_color_pairs[CLR_RED][CLR_RED] = colors_init_pair(color_pair++, FG_RED, BG_RED);
	_color_pairs[CLR_RED][CLR_GREEN] = colors_init_pair(color_pair++, FG_RED, BG_GRE);
	_color_pairs[CLR_RED][CLR_BLUE] = colors_init_pair(color_pair++, FG_RED, BG_BLU);
	_color_pairs[CLR_RED][CLR_YELLOW] = colors_init_pair(color_pair++, FG_RED, BG_YEL);

	_color_pairs[CLR_GREEN][CLR_RED] = colors_init_pair(color_pair++, FG_GRE, BG_RED);
	_color_pairs[CLR_GREEN][CLR_GREEN] = colors_init_pair(color_pair++, FG_GRE, BG_GRE);
	_color_pairs[CLR_GREEN][CLR_BLUE] = colors_init_pair(color_pair++, FG_GRE, BG_BLU);
	_color_pairs[CLR_GREEN][CLR_YELLOW] = colors_init_pair(color_pair++, FG_GRE, BG_YEL);

	_color_pairs[CLR_BLUE][CLR_RED] = colors_init_pair(color_pair++, FG_BLU, BG_RED);
	_color_pairs[CLR_BLUE][CLR_GREEN] = colors_init_pair(color_pair++, FG_BLU, BG_GRE);
	_color_pairs[CLR_BLUE][CLR_BLUE] = colors_init_pair(color_pair++, FG_BLU, BG_BLU);
	_color_pairs[CLR_BLUE][CLR_YELLOW] = colors_init_pair(color_pair++, FG_BLU, BG_YEL);

	_color_pairs[CLR_YELLOW][CLR_RED] = colors_init_pair(color_pair++, FG_YEL, BG_RED);
	_color_pairs[CLR_YELLOW][CLR_GREEN] = colors_init_pair(color_pair++, FG_YEL, BG_GRE);
	_color_pairs[CLR_YELLOW][CLR_BLUE] = colors_init_pair(color_pair++, FG_YEL, BG_BLU);
	_color_pairs[CLR_YELLOW][CLR_YELLOW] = colors_init_pair(color_pair++, FG_YEL, BG_YEL);
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
	// Initialize the color pairs with an undefined value.
	//
	color_pairs_init();

	//
	// Set the used color pairs.
	//
	colors_init_colors();

}

/******************************************************************************
 * The function fills a block array with random colors. The function ensures
 * that the center block has a color.
 *****************************************************************************/

#define colors_get_color() (rand() % 4) + 1

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
	wattrset(win, COLOR_PAIR(color_pair_get(CLR_NONE, bg_color)));
}

/******************************************************************************
 * The function sets the color for the info area if the current game ended.
 *****************************************************************************/

void colors_info_end_attr(WINDOW *win) {
	wattrset(win, A_BLINK| COLOR_PAIR(color_pair_get(CLR_RED,CLR_NONE)));
}

/******************************************************************************
 * The function sets the color.
 *****************************************************************************/

void colors_bg_attr(WINDOW *win, const t_block fg_color) {
	wattrset(win, COLOR_PAIR(color_pair_get(fg_color,CLR_NONE)));
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
		color_pair = color_pair_get(CLR_NONE, even ? CLR_GREY_LIGHT : CLR_GREY_DARK);

	} else {
		color_pair = color_pair_get(fg_color, bg_color);
	}

	log_debug("fg: %d bg: %d pair: %d", fg_color, bg_color, color_pair);

	wattrset(win, COLOR_PAIR(color_pair));
}

/******************************************************************************
 * The function determines the character to display for a given foreground and
 * background color. It is used for the game area.
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
