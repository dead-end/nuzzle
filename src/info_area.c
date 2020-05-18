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
#include "fs_persist.h"

/******************************************************************************
 * Define the output data.
 *****************************************************************************/

//
// The info area has 4 rows with each 12 columns.
//
#define ROWS 4

#define COLS 12

//
// We have an array that represents the rows and columns (including the
// terminating \0 character)
//
static char data[ROWS][COLS + 1];

//
// The index defines the index for the informations.
//
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

void info_area_init() {

	//
	// Read the high score from the score file.
	//
	high_score = fs_read_score();

	if (snprintf(data[IDX_HEAD], COLS + 1, "Nuzzle " VERSION) >= COLS + 1) {
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
 * The function prints the score and the high score. If the score changes, the
 * high score may change.
 *****************************************************************************/

static void info_area_print_score(WINDOW *win) {

	if (snprintf(data[IDX_HIGH], COLS + 1, "high: %6d", high_score) >= COLS + 1) {
		log_exit("Truncated: %s", data[IDX_HIGH]);
	}

	if (snprintf(data[IDX_SCORE], COLS + 1, "now:  %6d", cur_score) >= COLS + 1) {
		log_exit("Truncated: %s", data[IDX_SCORE]);
	}

	info_area_print(win);
}

/******************************************************************************
 * The function updates the current score, by adding something. After this the
 * updated info area has to be reprinted.
 *****************************************************************************/

void info_area_add_to_score(WINDOW *win, const int add_2_score) {

	cur_score += add_2_score;

	//
	// If the updated score is higher than the high score, we have to persist
	// the the new score and have to update the high score.
	//
	if (cur_score > high_score) {
		fs_write_score(cur_score);
		high_score = cur_score;
	}

	info_area_print_score(win);
}

/******************************************************************************
 * The function resets the score to 0..
 *****************************************************************************/

void info_area_reset_score(WINDOW *win) {

	cur_score = 0;

	info_area_print_score(win);
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

void info_area_print(WINDOW *win) {

	log_debug("row: %d col: %d", pos.row, pos.col);

	colors_normal_set_attr(win, CLR_NONE);

	for (int i = 0; i < ROWS; i++) {
		mvwprintw(win, pos.row + i, pos.col, data[i]);
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

void info_area_print_pixel(WINDOW *win, const s_point *pixel, t_block color) {

	const int row = pixel->row - pos.row;
	const int col = pixel->col - pos.col;

#ifdef DEBUG

	//
	// Ensure that the data is not null (which should not be the case.
	//
	if (data[row][col] == '\0') {
		log_exit("Data is null at: %d/%d", row, col);
	}

	log_debug("pixel: %d/%d '%c'", pixel->row, pixel->col, data[row][col]);
#endif

	colors_normal_set_attr(win, color);

	mvwprintw(win, pixel->row, pixel->col, "%c", data[row][col]);
}

/******************************************************************************
 * The function prints the status message with padding characters. This is a
 * first version, to show the end of the game.
 *
 * TODO: replace with menu / options
 *****************************************************************************/

void info_area_set_msg(WINDOW *win, const char *msg, const s_status *status) {

	if (snprintf(data[IDX_STATUS], COLS + 1, "%*s", COLS, msg) >= COLS + 1) {
		log_exit("Truncated: %s", data[IDX_STATUS]);
	}

	if (s_status_is_end(status)) {
		colors_normal_end_attr(win);
	} else {
		colors_normal_set_attr(win, CLR_NONE);
	}

	mvwprintw(win, pos.row + IDX_STATUS, pos.col, data[IDX_STATUS]);
}
