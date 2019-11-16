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

#include "info_area.h"

/******************************************************************************
 * Define the output data.
 *****************************************************************************/

#define ROWS 4

#define COLS 12

static char data[ROWS][COLS + 1];

#define IDX_HEAD 0

#define IDX_HIGH 1

#define IDX_SCORE 2

#define IDX_STATUS 3

/******************************************************************************
 * The variables contain the score informations.
 *****************************************************************************/

static int high_score = 0;

static int cur_score = 0;

/******************************************************************************
 * The struct contains the absolute position of the info area.
 *****************************************************************************/

static s_point pos;

/******************************************************************************
 * The function initializes the info area.
 *****************************************************************************/

void info_area_init(const int hs) {
	high_score = hs;

	if (snprintf(data[IDX_HEAD], COLS + 1, "Nuzzle 0.1.0") >= COLS + 1) {
		log_exit("Truncated: %s", data[IDX_HEAD]);
	}

	if (snprintf(data[IDX_HIGH], COLS + 1, "high: %6d", high_score) >= COLS + 1) {
		log_exit("Truncated: %s", data[IDX_HIGH]);
	}

	if (snprintf(data[IDX_SCORE], COLS + 1, "now:  %6d", cur_score) >= COLS + 1) {
		log_exit("Truncated: %s", data[IDX_SCORE]);
	}

	if (snprintf(data[IDX_STATUS], COLS + 1, "%*s", COLS, " ") >= COLS + 1) {
		log_exit("Truncated: %s", data[IDX_STATUS]);
	}
}

/******************************************************************************
 * The function updates the current score, by adding something. After this the
 * updated info area has to be reprinted.
 *****************************************************************************/

void info_area_add_to_score(const int add_2_score) {

	cur_score += add_2_score;

	if (snprintf(data[IDX_SCORE], COLS + 1, "now:  %6d", cur_score) >= COLS + 1) {
		log_exit("Truncated: %s", data[IDX_SCORE]);
	}

	info_area_print();
}

/******************************************************************************
 * The function returns a struct with the total size of the info area.
 *****************************************************************************/

s_point info_area_get_size() {
	s_point result;

	result.row = ROWS;
	result.col = COLS;

	log_debug("size row: %d col: %d", result.row, result.col);

	return result;
}

/******************************************************************************
 * The function sets the position of the info area. This is done on the
 * initialization and on resizing the terminal.
 *****************************************************************************/

void info_area_set_pos(const int row, const int col) {

	pos.row = row;
	pos.col = col;
}

/******************************************************************************
 * The function prints the info area at the absolute position.
 *****************************************************************************/

void info_area_print() {

	log_debug("row: %d col: %d", pos.row, pos.col);

	// TODO: color to colors.h ???
	attrset(COLOR_PAIR(CP_DEFAULT));

	for (int i = 0; i < ROWS; i++) {
		mvprintw(pos.row + i, pos.col, data[i]);
	}
}

/******************************************************************************
 * The function checks whether a pixel (terminal character) is inside the info
 * area.
 *****************************************************************************/

bool info_area_contains(const s_point *pixel) {

	//
	// Upper left corner
	//
	if (pixel->row < pos.row || pixel->col < pos.col) {
		return false;
	}

	//
	// Lower right corner
	//
	if (pixel->row >= pos.row + ROWS || pixel->col >= pos.col + COLS) {
		return false;
	}

	return true;
}

/******************************************************************************
 * The function prints a pixel (terminal character). It is assumed that the
 * pixel is inside the info area.
 *****************************************************************************/

void info_area_print_pixel(const s_point *pixel, enum e_colors color) {

	//TODO: called with above
	//
	// Ensure that the pixel is
	//
//	if (!info_area_contains(pixel)) {
//		log_exit("Wrong row: %d col: %d", pixel->row, pixel->col);
//	}

	const int row = pixel->row - pos.row;
	const int col = pixel->col - pos.col;

	colors_info_area_attr(color);

	log_debug("pixel: %d/%d '%c'", pixel->row, pixel->col, data[row][col]);

	// TODO: ensure that data[row][col] contains data other than \0
	mvprintw(pixel->row, pixel->col, "%c", data[row][col]);
}

/******************************************************************************
 * The function prints the status message with padding characters. This is a
 * first version, to show the end of the game.
 *****************************************************************************/

void info_area_set_msg(char *msg) {

	if (snprintf(data[IDX_STATUS], COLS + 1, "%*s", COLS, msg) >= COLS + 1) {
		log_exit("Truncated: %s", data[IDX_STATUS]);
	}

	attrset(COLOR_PAIR(CP_DEFAULT));

	mvprintw(pos.row + IDX_STATUS, pos.col, data[IDX_STATUS]);
}
