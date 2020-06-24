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
#include <score.h>

#include "info_area.h"

/******************************************************************************
 * Define of the array with the strings. The string array's have a fixed size.
 * The string itself can be shorter.
 *****************************************************************************/

//
// The info area has 5 rows. The size of a string array is the maximal size of
// the title of the game.
//
#define ROWS 5

#define COLS SIZE_TITLE

//
// We have an array that represents the rows and columns.
//
static char _data[ROWS][COLS];

//
// The index defines the index for the informations.
//
#define IDX_HEAD 0

#define IDX_TITLE 1

#define IDX_HIGH 2

#define IDX_SCORE 3

#define IDX_STATUS 4

/******************************************************************************
 * The variables contain the score informations.
 *****************************************************************************/

static int _high_score = 0;

static int _cur_score = 0;

/******************************************************************************
 * The struct contains the absolute position of the info area.
 *****************************************************************************/

static s_point _pos;

/******************************************************************************
 * The function initializes the info area. It is called each time a new game is
 * started (not only when the application is started).
 *****************************************************************************/

void info_area_init(const s_status *status) {

	//
	// Read the high score from the score file.
	//
	_high_score = score_read(status);

	_cur_score = 0;

	//
	// Initialize the data array with '\0'.
	//
	memset(_data, '\0', sizeof(_data));

	if (snprintf(_data[IDX_HEAD], COLS, "Nuzzle " VERSION) >= COLS) {
		log_exit("Truncated: %s", _data[IDX_HEAD]);
	}

	if (snprintf(_data[IDX_TITLE], COLS, "%s", status->game_cfg->title) >= COLS) {
		log_exit("Truncated: %s", _data[IDX_TITLE]);
	}

	if (snprintf(_data[IDX_HIGH], COLS, "high: %6d", _high_score) >= COLS) {
		log_exit("Truncated: %s", _data[IDX_HIGH]);
	}

	if (snprintf(_data[IDX_SCORE], COLS, "now:  %6d", _cur_score) >= COLS) {
		log_exit("Truncated: %s", _data[IDX_SCORE]);
	}

	//
	// IDX_STATUS has no value => nothing to do.
	//
}

/******************************************************************************
 * The function prints the score and the high score. If the score changes, the
 * high score may change.
 *****************************************************************************/

static void info_area_print_score(WINDOW *win) {

	if (snprintf(_data[IDX_HIGH], COLS, "high: %6d", _high_score) >= COLS) {
		log_exit("Truncated: %s", _data[IDX_HIGH]);
	}

	if (snprintf(_data[IDX_SCORE], COLS, "now:  %6d", _cur_score) >= COLS) {
		log_exit("Truncated: %s", _data[IDX_SCORE]);
	}

	info_area_print(win);
}

/******************************************************************************
 * The function updates the current score, by adding something. After this the
 * updated info area has to be reprinted.
 *****************************************************************************/

void info_area_add_to_score(WINDOW *win, const s_status *status, const int add_2_score) {

	_cur_score += add_2_score;

	//
	// If the updated score is higher than the high score, we have to persist
	// the the new score and have to update the high score.
	//
	if (_cur_score > _high_score) {
		score_write(status, _cur_score);
		_high_score = _cur_score;
	}

	info_area_print_score(win);
}

/******************************************************************************
 * The function returns a struct with the effective size of the info area.
 *****************************************************************************/

s_point info_area_get_size() {
	s_point result;

	//
	// The number of rows is fixed.
	//
	result.row = ROWS;

	//
	// We compute the maximal string length inside the array of strings.
	//
	result.col = 0;

	for (int i = 0; i < ROWS; i++) {

		//
		// Get the string length of the row.
		//
		const int len = strlen(_data[i]);

		//
		// If the length is higher than the current maximal length, we have to
		// update it.
		//
		if (len > result.col) {
			result.col = len;
			log_debug("len %d str: %s", len, _data[i]);
		}
	}

	log_debug("size row: %d col: %d", result.row, result.col);

	return result;
}

/******************************************************************************
 * The function sets the position of the info area. This is done on the
 * initialization and on resizing the terminal.
 *****************************************************************************/

void info_area_set_pos(const int row, const int col) {

	_pos.row = row;
	_pos.col = col;
}

/******************************************************************************
 * The function prints the info area at the absolute position.
 *****************************************************************************/

void info_area_print(WINDOW *win) {

	log_debug("row: %d col: %d", _pos.row, _pos.col);

	colors_normal_set_attr(win, CLR_NONE);

	for (int i = 0; i < ROWS; i++) {
		mvwprintw(win, _pos.row + i, _pos.col, _data[i]);
	}
}

/******************************************************************************
 * The function checks whether a pixel (terminal character) is inside the info
 * area. The function uses the maximal area not the currently used.
 *****************************************************************************/

bool info_area_contains(const s_point *pixel) {

	//
	// Upper left corner
	//
	if (pixel->row < _pos.row || pixel->col < _pos.col) {
		return false;
	}

	//
	// Lower right corner (The columns have a terminating '\0')
	//
	if (pixel->row >= _pos.row + ROWS || pixel->col >= _pos.col + COLS - 1) {
		return false;
	}

	return true;
}

/******************************************************************************
 * The function prints a pixel (terminal character). It is assumed that the
 * pixel is inside the info area.
 *****************************************************************************/

void info_area_print_pixel(WINDOW *win, const s_point *pixel, const t_block color) {

	const int row = pixel->row - _pos.row;
	const int col = pixel->col - _pos.col;

	const char c = _data[row][col] == '\0' ? ' ' : _data[row][col];

	colors_normal_set_attr(win, color);

	mvwprintw(win, pixel->row, pixel->col, "%c", c);
}

/******************************************************************************
 * The function prints the status message with padding characters. This is a
 * first version, to show the end of the game.
 *
 * TODO: replace with menu / options
 *****************************************************************************/

void info_area_set_msg(WINDOW *win, const char *msg, const s_status *status) {
	const char *fmt;

	if (s_status_is_end(status)) {
		colors_normal_end_attr(win);
		fmt = "** %s **";

	} else {
		colors_normal_set_attr(win, CLR_NONE);
		fmt = "%s";
	}

	if (snprintf(_data[IDX_STATUS], COLS, fmt, msg) >= COLS) {
		log_exit("Truncated: %s", _data[IDX_STATUS]);
	}

	mvwprintw(win, _pos.row + IDX_STATUS, _pos.col, _data[IDX_STATUS]);
}
