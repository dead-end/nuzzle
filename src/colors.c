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

#define BG_L_G 16
#define BG_M_G 17
#define BG_D_G 18

#define FG_RED 19
#define BG_RED 20

#define FG_GRE 21
#define BG_GRE 22

#define FG_BLU 23
#define BG_BLU 24

#define FG_YEL 25
#define BG_YEL 26

#define MK_DRK 27
#define MK_MID 28
#define MK_LIG 29

/******************************************************************************
 * The definition of the color constants.
 *****************************************************************************/

#define FULL 999
#define CORR 100
#define LIGH 600
#define DARK 300

#define MID_ 700

/******************************************************************************
 * The color pairs are stored in a 2 dimensional array. The indices are the
 * color indices for the foreground and the background.
 *****************************************************************************/

//
// The default color pair is unchanged.
//
#define CP_DEFAULT 0

#define CP_START 8

#define CP_UNDEF -1

//
// The number of colors for the foreground and the background:
//
// - none / black
// - 3 x grey
// - 4 colors
// - 3 marker
//
#define NUM_COLORS 11

static t_block _color_pairs[NUM_COLORS][NUM_COLORS];

/******************************************************************************
 * The function initializes the array with the color pairs. They are set to an
 * undefined value. Not all combinations of color pairs are necessary.
 *****************************************************************************/

static void color_pairs_undef() {

	for (int fg = 0; fg < NUM_COLORS; fg++) {
		for (int bg = 0; bg < NUM_COLORS; bg++) {
			_color_pairs[fg][bg] = CP_UNDEF;
		}
	}
}

/******************************************************************************
 * The function returns a color pair for a pair of colors. It simply returns
 * the value of the _color_pairs array. Because not all color pair combinations
 * are defined, we check if the combination is valid in debug mode.
 *****************************************************************************/

static inline t_block color_pair_get(const short fg, const short bg) {

#ifdef DEBUG

	//
	// Ensure that the color indices are in a valid range.
	//
	if (fg < 0 || fg >= NUM_COLORS || bg < 0 || bg >= NUM_COLORS) {
		log_exit("Invalid color index: %d fg: %d bg: %d", NUM_COLORS, fg, bg);
	}
#endif

	const t_block cp = _color_pairs[fg][bg];

#ifdef DEBUG
	log_debug("Color pair: %d fg: %d bg: %d", cp, fg, bg);

	if (cp == CP_UNDEF) {
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
 * The function initializes the necessary colors. Black and white are not
 * created.
 *****************************************************************************/

static void colors_alloc() {

	//
	// Chess pattern colors
	//
	colors_init_color(BG_L_G, 250, 250, 250);
	colors_init_color(BG_M_G, 200, 200, 200);
	colors_init_color(BG_D_G, 150, 150, 150);

	//
	// Red
	//
	colors_init_color(FG_RED, FULL, DARK, DARK);
	colors_init_color(BG_RED, FULL, LIGH, LIGH);

	//
	// Green
	//
	colors_init_color(FG_GRE, DARK, FULL - CORR - CORR, DARK);
	colors_init_color(BG_GRE, LIGH, FULL, LIGH);

	//
	// Blue
	//
	colors_init_color(FG_BLU, DARK, DARK, FULL);
	colors_init_color(BG_BLU, LIGH, LIGH, FULL);

	//
	// Yellow
	//
	colors_init_color(FG_YEL, FULL - CORR, FULL - CORR, DARK);
	colors_init_color(BG_YEL, FULL, FULL, LIGH);

	//
	// Marker
	//
	colors_init_color(MK_DRK, 500, 0, 300);
	colors_init_color(MK_MID, 800, 100, 500);
	colors_init_color(MK_LIG, 999, 200, 700);
}

/******************************************************************************
 * The function initializes the necessary color pairs. The default color pair
 * (black and white) is used.
 *****************************************************************************/

static void color_pairs_alloc() {

	//
	// We use an offset to keep the first color pairs.
	//
	short color_pair = CP_START;

	_color_pairs[CLR_NONE][CLR_NONE] = CP_DEFAULT;

	//
	// Initialize color pairs for the chess pattern.
	//
	_color_pairs[CLR_NONE][CLR_GREY_LIGHT] = colors_init_pair(color_pair++, BG_L_G, BG_L_G);
	_color_pairs[CLR_NONE][CLR_GREY_MID__] = colors_init_pair(color_pair++, BG_M_G, BG_M_G);
	_color_pairs[CLR_NONE][CLR_GREY_DARK_] = colors_init_pair(color_pair++, BG_D_G, BG_D_G);

	//
	// Initialize the color pairs with black.
	//
	_color_pairs[CLR_RED_][CLR_NONE] = colors_init_pair(color_pair++, FG_RED, COLOR_BLACK);
	_color_pairs[CLR_GREE][CLR_NONE] = colors_init_pair(color_pair++, FG_GRE, COLOR_BLACK);
	_color_pairs[CLR_BLUE][CLR_NONE] = colors_init_pair(color_pair++, FG_BLU, COLOR_BLACK);
	_color_pairs[CLR_YELL][CLR_NONE] = colors_init_pair(color_pair++, FG_YEL, COLOR_BLACK);

	_color_pairs[CLR_NONE][CLR_RED_] = colors_init_pair(color_pair++, COLOR_BLACK, BG_RED);
	_color_pairs[CLR_NONE][CLR_GREE] = colors_init_pair(color_pair++, COLOR_BLACK, BG_GRE);
	_color_pairs[CLR_NONE][CLR_BLUE] = colors_init_pair(color_pair++, COLOR_BLACK, BG_BLU);
	_color_pairs[CLR_NONE][CLR_YELL] = colors_init_pair(color_pair++, COLOR_BLACK, BG_YEL);

	//
	// Initialize the color pairs for the rgby combinations.
	//
	_color_pairs[CLR_RED_][CLR_RED_] = colors_init_pair(color_pair++, FG_RED, BG_RED);
	_color_pairs[CLR_RED_][CLR_GREE] = colors_init_pair(color_pair++, FG_RED, BG_GRE);
	_color_pairs[CLR_RED_][CLR_BLUE] = colors_init_pair(color_pair++, FG_RED, BG_BLU);
	_color_pairs[CLR_RED_][CLR_YELL] = colors_init_pair(color_pair++, FG_RED, BG_YEL);

	_color_pairs[CLR_GREE][CLR_RED_] = colors_init_pair(color_pair++, FG_GRE, BG_RED);
	_color_pairs[CLR_GREE][CLR_GREE] = colors_init_pair(color_pair++, FG_GRE, BG_GRE);
	_color_pairs[CLR_GREE][CLR_BLUE] = colors_init_pair(color_pair++, FG_GRE, BG_BLU);
	_color_pairs[CLR_GREE][CLR_YELL] = colors_init_pair(color_pair++, FG_GRE, BG_YEL);

	_color_pairs[CLR_BLUE][CLR_RED_] = colors_init_pair(color_pair++, FG_BLU, BG_RED);
	_color_pairs[CLR_BLUE][CLR_GREE] = colors_init_pair(color_pair++, FG_BLU, BG_GRE);
	_color_pairs[CLR_BLUE][CLR_BLUE] = colors_init_pair(color_pair++, FG_BLU, BG_BLU);
	_color_pairs[CLR_BLUE][CLR_YELL] = colors_init_pair(color_pair++, FG_BLU, BG_YEL);

	_color_pairs[CLR_YELL][CLR_RED_] = colors_init_pair(color_pair++, FG_YEL, BG_RED);
	_color_pairs[CLR_YELL][CLR_GREE] = colors_init_pair(color_pair++, FG_YEL, BG_GRE);
	_color_pairs[CLR_YELL][CLR_BLUE] = colors_init_pair(color_pair++, FG_YEL, BG_BLU);
	_color_pairs[CLR_YELL][CLR_YELL] = colors_init_pair(color_pair++, FG_YEL, BG_YEL);

	//
	// Initialize the color pairs for marker / double chess combinations.
	//
	_color_pairs[CLR_NONE][CLR_MK_N] = colors_init_pair(color_pair++, COLOR_BLACK, MK_DRK);

	_color_pairs[CLR_MK_L][CLR_NONE] = colors_init_pair(color_pair++, MK_LIG, COLOR_BLACK);
	_color_pairs[CLR_MK_N][CLR_NONE] = colors_init_pair(color_pair++, MK_MID, COLOR_BLACK);

	_color_pairs[CLR_MK_L][CLR_MK_N] = colors_init_pair(color_pair++, MK_LIG, MK_DRK);
	_color_pairs[CLR_MK_N][CLR_MK_N] = colors_init_pair(color_pair++, MK_MID, MK_DRK);
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
	// Allocate the necessary colors.
	//
	colors_alloc();

	//
	// Initialize the color pairs with an undefined value.
	//
	color_pairs_undef();

	//
	// Allocate the necessary color pairs.
	//
	color_pairs_alloc();
}

/******************************************************************************
 * The function initializes the _random value, which will be configured in the
 * game cfg file.
 *****************************************************************************/

static int _random;

void colors_setup_init_random(const char *data) {
	_random = str_2_int(data);

	if (_random < 1 || _random > 100) {
		log_exit("Random value is invalid: %d (allowed: 1 - 100)", _random);
	}

	log_debug("Random: %d", _random);
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
			if (rand() % 100 < _random) {
				blocks[row][col] = CLR_NONE;

			} else {
				blocks[row][col] = colors_get_color();
			}

			log_debug("block: %d/%d color: %d", row, col, blocks[row][col]);
		}
	}
}

/******************************************************************************
 * The function sets the color for the normal areas (no chess pattern). It is
 * called with the color of the drop area, which is the background color. The
 * foreground color is CLR_NONE, which means the default color.
 *****************************************************************************/

void colors_normal_set_attr(WINDOW *win, const t_block da_color) {
	wattrset(win, COLOR_PAIR(color_pair_get(CLR_NONE, da_color)));
}

/******************************************************************************
 * The function sets the color for the info area if the current game ended.
 *****************************************************************************/

void colors_normal_end_attr(WINDOW *win) {
	wattrset(win, A_BLINK| COLOR_PAIR(color_pair_get(CLR_RED_,CLR_NONE)));
}

/******************************************************************************
 * The function returns the background chess pattern color for a given index.
 *****************************************************************************/

static short colors_chess_get_color(const s_point *idx, const e_chess_type chess_type) {

	//
	// Simple chess pattern with 2 dark colors
	//
	if (chess_type == CHESS_SIMPLE_DARK) {
		return colors_is_even(idx->row, idx->col) ? CLR_GREY_MID__ : CLR_GREY_DARK_;
	}

	//
	// Simple chess pattern with 2 light colors
	//
	if (chess_type == CHESS_SIMPLE_LIGHT) {
		return colors_is_even(idx->row, idx->col) ? CLR_GREY_LIGHT : CLR_GREY_MID__;
	}

	//
	// Double chess pattern. The even blocks have the middle color.
	//
	if (colors_is_even(idx->row, idx->col)) {
		return CLR_GREY_MID__;
	}

	//
	// Toggle the odd blocks.
	//
	if (colors_is_even((idx->row / 3), (idx->col / 3))) {
		return CLR_GREY_LIGHT;
	}

	return CLR_GREY_DARK_;
}

/******************************************************************************
 * The function sets the color pair for a chess pattern area and returns the
 * corresponding character. It is called with the game area and the drop area
 * color index and a flag indicating whether the block is odd or even.
 *****************************************************************************/

wchar_t colors_chess_attr_char(WINDOW *win, const t_block ga_color, const t_block da_color, const s_point *idx, const e_chess_type chess_type) {
	int color_pair;
	wchar_t chr;

	if (ga_color == CLR_NONE) {

		//
		// Both are empty
		//
		if (da_color == CLR_NONE) {
			chr = BLOCK_EMPTY;
			color_pair = color_pair_get(CLR_NONE, colors_chess_get_color(idx, chess_type));
		}

		//
		// The drop area has a color and the drop area is displayed as a
		// background color.
		//
		else {
			chr = BLOCK_EMPTY;
			color_pair = color_pair_get(ga_color, da_color);
		}
	} else {

		//
		// The foreground is set, so we need a full character.
		//
		if (da_color == CLR_NONE) {
			chr = BLOCK_FULL;
			color_pair = color_pair_get(ga_color, da_color);

		}

		//
		// Both colors are set, so we need a transparent character.
		//
		else {
			chr = BLOCK_BOTH;
			color_pair = color_pair_get(ga_color, da_color);
		}
	}

	log_debug("fg: %d bg: %d pair: %d char '%lc", da_color, ga_color, color_pair, chr);

	wattrset(win, COLOR_PAIR(color_pair));

	return chr;
}
