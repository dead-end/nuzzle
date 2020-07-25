/*
 * MIT License
 *
 * Copyright (c) 2020 dead-end
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
#include <stdio.h>
#include <linux/limits.h>
#include <errno.h>

#include "colors.h"
#include "file_system.h"

/******************************************************************************
 * Definitions.
 *****************************************************************************/

#define BUF_SIZE 1024

#define COLOR_CFG "color.cfg"

/******************************************************************************
 * A color is defined by a r,g,b value. Each have a range between 0 and 999.
 *****************************************************************************/

#define COLOR_VALUE_MIN 0

#define COLOR_VALUE_MAX 999

/******************************************************************************
 * The definition of the colors, that are used inside this game.
 *****************************************************************************/

#define COL_DEF_OFFSET 16

#define COL_DEF_NUM 17

//
// "default.fg", "default.bg"
//
#define DEFAULT_FG 16
#define DEFAULT_BG 17

//
// "chess.light", "chess.middle", "chess.dark",
//
#define CHESS_LG 18
#define CHESS_MD 19
#define CHESS_DK 20

//
// "red.light", "red.dark", "red.bg",
//
#define RED_LG 21
#define RED_DK 22
#define RED_BG 23

//
// "green.light", "green.dark", "green.bg",
//
#define GREEN_LG 24
#define GREEN_DK 25
#define GREEN_BG 26

//
// "blue.light", "blue.dark", "blue.bg",
//
#define BLUE_LG 27
#define BLUE_DK 28
#define BLUE_BG 29

//
// "yellow.light", "yellow.dark", "yellow.bg"
//
#define YELLOW_LG 30
#define YELLOW_DK 31
#define YELLOW_BG 32

//
// The corresponding names for the config file.
//
static const char *_color_def_names[COL_DEF_NUM] = {

"default.fg", "default.bg",

"chess.light", "chess.middle", "chess.dark",

"red.light", "red.dark", "red.bg",

"green.light", "green.dark", "green.bg",

"blue.light", "blue.dark", "blue.bg",

"yellow.light", "yellow.dark", "yellow.bg"

};

/******************************************************************************
 * The color pairs are stored in a 2 dimensional array. The indices are the
 * color indices for the foreground and the background.
 *****************************************************************************/

//
// The default color pair is unchanged.
//
#define CP_START   8

#define CP_UNDEF  -1

//
// The number of colors for the foreground and the background:
//
// - none / black
// - 3 x grey
// - 4 normal and 4 light colors
//
#define NUM_COLORS 12

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

	//
	// Ensure that the color pair is defined.
	//
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

/*******************************************************************************
 * The function initializes the color definition array with -1.
 ******************************************************************************/

static void color_def_init(int c_defs[][3], const int num) {

	for (int i = 0; i < num; i++) {

		c_defs[i][0] = -1;
		c_defs[i][1] = -1;
		c_defs[i][2] = -1;
	}
}

/*******************************************************************************
 * The function checks the array of color definitions. The array was initialized
 * with -1. After the configuration file was read, all the -1 values should be
 * overwritten. This is checked here.
 ******************************************************************************/

static void color_def_check(int c_defs[][3], const int num, const char *c_defs_names[]) {

	for (int i = 0; i < num; i++) {

		if (c_defs[i][0] == -1 || c_defs[i][1] == -1 || c_defs[i][2] == -1) {
			log_exit("Color definition not valid: %s", c_defs_names[i]);
		}
	}
}

/*******************************************************************************
 * The function allocates the colors defined in the array. Each color has an id
 * which is the index in the array and an offset.
 ******************************************************************************/

static void color_def_allocate(int c_defs[][3], const int num, const int offset) {

	for (int i = 0; i < num; i++) {

		colors_init_color(i + offset, c_defs[i][0], c_defs[i][1], c_defs[i][2]);
	}
}

/*******************************************************************************
 * The function pares and checks a line from the configuration file. It is
 * assumed, that the file starts with the prefix.
 ******************************************************************************/

static void color_def_parse(const char *line, const char *prefix, int *result) {
	const int len = strlen(prefix);

	sscanf(&line[len], "=%d,%d,%d", &result[0], &result[1], &result[2]);

	if (result[0] < COLOR_VALUE_MIN || result[0] > COLOR_VALUE_MAX) {
		log_exit("First value not valid - line: %s", line);
	}

	if (result[1] < COLOR_VALUE_MIN || result[1] > COLOR_VALUE_MAX) {
		log_exit("Second value not valid - line: %s", line);
	}

	if (result[2] < COLOR_VALUE_MIN || result[2] > COLOR_VALUE_MAX) {
		log_exit("Third value not valid - line: %s", line);
	}

	log_debug("color: %s %3d, %3d, %3d", prefix, result[0], result[1],result[2]);
}

/*******************************************************************************
 * The function processes the file with the color definitions. Each line of the
 * file contains a key and 3 int values, which represent red, green, blue.
 ******************************************************************************/

static void color_def_process_file(FILE *file, const char *path) {
	char line[BUF_SIZE];

	int color_defs[COL_DEF_NUM][3];

	bool found;

	//
	// Initialize the array of color definitions with -1. If one of the color
	// values is -1, we know that it was not set.
	//
	color_def_init(color_defs, COL_DEF_NUM);

	//
	// Read the file line by line.
	//
	while (fgets(line, BUF_SIZE, file) != NULL) {

		//
		// Ensure that the IO operation succeeded.
		//
		if (ferror(file)) {
			log_exit("Unable to read file: %s - %s", path, strerror(errno));
		}

		//
		// Remove tailing white spaces.
		//
		trim_r(line);

		//
		// Ignore empty lines and comments.
		//
		if (strlen(line) == 0 || line[0] == '#') {
			continue;
		}

		log_debug("line: %s", line);

		found = false;

		//
		// Loop through the color definition names.
		//
		for (int i = 0; i < COL_DEF_NUM; i++) {

			//
			// If we found the correct definition, we can parse the result.
			//
			if (starts_with(line, _color_def_names[i])) {
				color_def_parse(line, _color_def_names[i], color_defs[i]);
				found = true;
				break;
			}
		}

		//
		// Ensure that the line contains something useful.
		//
		if (!found) {
			log_exit("Unknown definition: %s", line);
		}
	}

	//
	// Ensure that all colors are defined, by checking that there is no -1
	// value.
	//
	color_def_check(color_defs, COL_DEF_NUM, _color_def_names);

	//
	// Allocate the colors.
	//
	color_def_allocate(color_defs, COL_DEF_NUM, COL_DEF_OFFSET);
}

/*******************************************************************************
 * The function does the IO stuff for reading the configuration file with the
 * color definitions. The processing of the file is done in a separate function.
 ******************************************************************************/

void colors_alloc() {

	char path[PATH_MAX];

	//
	//Find the file in one of the configuration directories.
	//
	if (!fs_get_cfg_file(COLOR_CFG, path, PATH_MAX)) {
		log_exit("No config file found: %s", COLOR_CFG);
	}

	//
	// Open the configuration file.
	//
	FILE *file = fopen(path, "r");
	if (file == NULL) {
		log_exit("Unable open file: %s - %s", path, strerror(errno));
	}

	//
	// Delegate the processing to a separate function.
	//
	color_def_process_file(file, path);

	//
	// Close the file and check for errors.
	//
	if (fclose(file) == -1) {
		log_exit("Unable close file: %s - %s", path, strerror(errno));
	}
}

/******************************************************************************
 * The function initializes the necessary color pairs. The default color pair
 * (black and white) is used.
 *****************************************************************************/

static void color_pairs_alloc() {

	//
	// We redefine the default color pair.
	//
	if (assume_default_colors(DEFAULT_FG, DEFAULT_BG) == ERR) {
		log_exit_str("Unable to change default colors.");
	}

	//
	// And we use the default color pair (which is 0)
	//
	_color_pairs[CLR_NONE][CLR_NONE] = 0;

	//
	// We use an offset to keep the first color pairs.
	//
	short color_pair = CP_START;

	//
	// Initialize color pairs for the chess pattern.
	//
	_color_pairs[CLR_NONE][CLR_GREY_LIGHT] = colors_init_pair(color_pair++, CHESS_LG, CHESS_LG);
	_color_pairs[CLR_NONE][CLR_GREY_MID__] = colors_init_pair(color_pair++, CHESS_MD, CHESS_MD);
	_color_pairs[CLR_NONE][CLR_GREY_DARK_] = colors_init_pair(color_pair++, CHESS_DK, CHESS_DK);

	//
	// Initialize the color pairs with black.
	//
	_color_pairs[CLR_RED__N][CLR_NONE] = colors_init_pair(color_pair++, RED_DK, DEFAULT_BG);
	_color_pairs[CLR_GREE_N][CLR_NONE] = colors_init_pair(color_pair++, GREEN_DK, DEFAULT_BG);
	_color_pairs[CLR_BLUE_N][CLR_NONE] = colors_init_pair(color_pair++, BLUE_DK, DEFAULT_BG);
	_color_pairs[CLR_YELL_N][CLR_NONE] = colors_init_pair(color_pair++, YELLOW_DK, DEFAULT_BG);

	//
	// Initialize the color pairs with foreground CLR_NONE. They are used for
	// the info area. The foreground color is the color of the writings.
	//
	_color_pairs[CLR_NONE][CLR_RED__N] = colors_init_pair(color_pair++, DEFAULT_FG, RED_BG);
	_color_pairs[CLR_NONE][CLR_GREE_N] = colors_init_pair(color_pair++, DEFAULT_FG, GREEN_BG);
	_color_pairs[CLR_NONE][CLR_BLUE_N] = colors_init_pair(color_pair++, DEFAULT_FG, BLUE_BG);
	_color_pairs[CLR_NONE][CLR_YELL_N] = colors_init_pair(color_pair++, DEFAULT_FG, YELLOW_BG);

	//
	// Initialize the color pairs for the rgby combinations.
	//
	_color_pairs[CLR_RED__N][CLR_RED__N] = colors_init_pair(color_pair++, RED_DK, RED_BG);
	_color_pairs[CLR_RED__N][CLR_GREE_N] = colors_init_pair(color_pair++, RED_DK, GREEN_BG);
	_color_pairs[CLR_RED__N][CLR_BLUE_N] = colors_init_pair(color_pair++, RED_DK, BLUE_BG);
	_color_pairs[CLR_RED__N][CLR_YELL_N] = colors_init_pair(color_pair++, RED_DK, YELLOW_BG);

	_color_pairs[CLR_GREE_N][CLR_RED__N] = colors_init_pair(color_pair++, GREEN_DK, RED_BG);
	_color_pairs[CLR_GREE_N][CLR_GREE_N] = colors_init_pair(color_pair++, GREEN_DK, GREEN_BG);
	_color_pairs[CLR_GREE_N][CLR_BLUE_N] = colors_init_pair(color_pair++, GREEN_DK, BLUE_BG);
	_color_pairs[CLR_GREE_N][CLR_YELL_N] = colors_init_pair(color_pair++, GREEN_DK, YELLOW_BG);

	_color_pairs[CLR_BLUE_N][CLR_RED__N] = colors_init_pair(color_pair++, BLUE_DK, RED_BG);
	_color_pairs[CLR_BLUE_N][CLR_GREE_N] = colors_init_pair(color_pair++, BLUE_DK, GREEN_BG);
	_color_pairs[CLR_BLUE_N][CLR_BLUE_N] = colors_init_pair(color_pair++, BLUE_DK, BLUE_BG);
	_color_pairs[CLR_BLUE_N][CLR_YELL_N] = colors_init_pair(color_pair++, BLUE_DK, YELLOW_BG);

	_color_pairs[CLR_YELL_N][CLR_RED__N] = colors_init_pair(color_pair++, YELLOW_DK, RED_BG);
	_color_pairs[CLR_YELL_N][CLR_GREE_N] = colors_init_pair(color_pair++, YELLOW_DK, GREEN_BG);
	_color_pairs[CLR_YELL_N][CLR_BLUE_N] = colors_init_pair(color_pair++, YELLOW_DK, BLUE_BG);
	_color_pairs[CLR_YELL_N][CLR_YELL_N] = colors_init_pair(color_pair++, YELLOW_DK, YELLOW_BG);

	//
	// Initialize the color pairs for light colors.
	//
	_color_pairs[CLR_RED__L][CLR_NONE] = colors_init_pair(color_pair++, RED_LG, DEFAULT_BG);
	_color_pairs[CLR_RED__L][CLR_RED__N] = colors_init_pair(color_pair++, RED_LG, RED_BG);

	_color_pairs[CLR_GREE_L][CLR_NONE] = colors_init_pair(color_pair++, GREEN_LG, DEFAULT_BG);
	_color_pairs[CLR_GREE_L][CLR_GREE_N] = colors_init_pair(color_pair++, GREEN_LG, GREEN_BG);

	_color_pairs[CLR_BLUE_L][CLR_NONE] = colors_init_pair(color_pair++, BLUE_LG, DEFAULT_BG);
	_color_pairs[CLR_BLUE_L][CLR_BLUE_N] = colors_init_pair(color_pair++, BLUE_LG, BLUE_BG);

	_color_pairs[CLR_YELL_L][CLR_NONE] = colors_init_pair(color_pair++, YELLOW_LG, DEFAULT_BG);
	_color_pairs[CLR_YELL_L][CLR_YELL_N] = colors_init_pair(color_pair++, YELLOW_LG, YELLOW_BG);
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
	wattrset(win, A_BLINK| COLOR_PAIR(color_pair_get(CLR_RED__N,CLR_NONE)));
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

wchar_t colors_chess_attr_char(WINDOW *win, t_block ga_color, const t_block da_color, const s_point *idx, const e_chess_type chess_type) {
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
		// If CHESS_DOUBLE is active we assume that ga_color is normal color.
		//
		if (chess_type == CHESS_DOUBLE && colors_is_even((idx->row / 3), (idx->col / 3))) {

			//
			// ATTENTION: Parameter is reset with a light color
			//
			ga_color = colors_get_light(ga_color);
		}

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
