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
 *****************************************************************************/

//
// The info area is surrounded by a box. So the first and the last row are part
// of the box.
//
#define L_ROWS 7

//
// The column size is the size of the title plus the box with a padding space.
//
#define L_COLS SIZE_TITLE + 4

//
// The inner size including the terminating \0
//
#define size_line_get() L_COLS

#define size_inner_get() (L_COLS - 4)

//
// We have an array that represents the rows and columns.
//
static wchar_t _data[L_ROWS][L_COLS];

/******************************************************************************
 * The index definitions for the lines.
 *****************************************************************************/

#define IDX_TOP 0

#define IDX_TITLE 1

#define IDX_GAME 2

#define IDX_HIGH 3

#define IDX_SCORE 4

#define IDX_STATUS 5

#define IDX_BOTTOM 6

#define INNER_START IDX_TITLE

#define INNER_END IDX_STATUS

/******************************************************************************
 * The format definitions for the lines.
 *****************************************************************************/

#define FMT_TITLE L"Nuzzle    - v%s"

#define FMT_GAME  L"%s"

#define FMT_HIGH  L"High score:   %4d"

#define FMT_SCORE L"Current   :   %4d"

#define FMT_TURN  L"Turn      :   %4d"

#define FMT_END   L"+++ END +++"

/******************************************************************************
 * The variables contain the score informations.
 *****************************************************************************/

static int _high_score = 0;

static int _cur_score = 0;

static int _turn = 0;

/******************************************************************************
 * The struct contains the absolute position of the info area.
 *****************************************************************************/

static s_point _pos;

/******************************************************************************
 * The function returns a struct with the effective size of the info area.
 *****************************************************************************/

s_point info_area_get_size() {
	s_point result = { L_ROWS, L_COLS - 1 };
	return result;
}

/******************************************************************************
 * The function writes a box line to a buffer. The size of the buffer has to be
 * at least "size". The function is called with a start, and end and a padding
 * character.
 *
 * (Unit tested)
 *****************************************************************************/

void cp_box_line(wchar_t *dst, const int size, const wchar_t start, const wchar_t end, const wchar_t pad) {

	wmemset(&dst[1], pad, size - 3);

	dst[0] = start;
	dst[size - 2] = end;
	dst[size - 1] = L'\0';
}

/******************************************************************************
 * Add the borders to the inner lines
 *****************************************************************************/

static void add_border(wchar_t *dst, const int size, const wchar_t border, const wchar_t pad) {

	dst[0] = border;
	dst[1] = pad;

#ifdef DEBUG

	//
	// We are overwriting the terminating \0 from the inner string.
	//
	if (dst[size - 3] != U_TERM) {
		log_exit("Strange: %ls", dst);
	}
#endif

	dst[size - 3] = pad;
	dst[size - 2] = border;
	dst[size - 1] = U_TERM;
}

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

	_turn = 1;

	cp_box_line(_data[IDX_TOP], size_line_get(), U_ULCORNER, U_URCORNER, U_HLINE);
	cp_box_line(_data[IDX_BOTTOM], size_line_get(), U_LLCORNER, U_LRCORNER, U_HLINE);

	//
	// Title
	//
	fmt_center(&_data[IDX_TITLE][2], size_inner_get(), U_EMPTY, FMT_TITLE, VERSION);
	add_border(_data[IDX_TITLE], size_line_get(), U_VLINE, U_EMPTY);

	//
	// Game
	//
	fmt_center(&_data[IDX_GAME][2], size_inner_get(), U_EMPTY, FMT_GAME, status->game_cfg->title);
	add_border(_data[IDX_GAME], size_line_get(), U_VLINE, U_EMPTY);

	//
	// High score
	//
	fmt_center(&_data[IDX_HIGH][2], size_inner_get(), U_EMPTY, FMT_HIGH, _high_score);
	add_border(_data[IDX_HIGH], size_line_get(), U_VLINE, U_EMPTY);

	//
	// Current score
	//
	fmt_center(&_data[IDX_SCORE][2], size_inner_get(), U_EMPTY, FMT_SCORE, _cur_score);
	add_border(_data[IDX_SCORE], size_line_get(), U_VLINE, U_EMPTY);

	//
	// Status
	//
	fmt_center(&_data[IDX_STATUS][2], size_inner_get(), U_EMPTY, FMT_TURN, _turn);
	add_border(_data[IDX_STATUS], size_line_get(), U_VLINE, U_EMPTY);
}

/******************************************************************************
 * The function prints the score and the high score. If the score changes, the
 * high score may change.
 *****************************************************************************/

static void info_area_print_score(WINDOW *win, const s_status *status) {

	//
	// High score
	//
	fmt_center(&_data[IDX_HIGH][2], size_inner_get(), U_EMPTY, FMT_HIGH, _high_score);
	add_border(_data[IDX_HIGH], size_line_get(), U_VLINE, U_EMPTY);

	//
	// Current score
	//
	fmt_center(&_data[IDX_SCORE][2], size_inner_get(), U_EMPTY, FMT_SCORE, _cur_score);
	add_border(_data[IDX_SCORE], size_line_get(), U_VLINE, U_EMPTY);

	//
	// Status
	//
	fmt_center(&_data[IDX_STATUS][2], size_inner_get(), U_EMPTY, FMT_TURN, _turn);
	add_border(_data[IDX_STATUS], size_line_get(), U_VLINE, U_EMPTY);

	info_area_print(win, status);
}

/******************************************************************************
 * The function updates the current score, by adding something. After this the
 * updated info area has to be reprinted.
 *****************************************************************************/

void info_area_update_score_turns(WINDOW *win, const s_status *status, const int add_2_score) {

	_cur_score += add_2_score;

	//
	// If the updated score is higher than the high score, we have to persist
	// the the new score and have to update the high score.
	//
	if (_cur_score > _high_score) {
		score_write(status, _cur_score);
		_high_score = _cur_score;
	}

	_turn++;

	info_area_print_score(win, status);
}

/******************************************************************************
 * The function updates the turns.
 *****************************************************************************/

void info_area_new_turn(WINDOW *win, const s_status *status) {

	_turn++;

	info_area_print_score(win, status);
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
 * The function prints an inner line.
 *****************************************************************************/

static void info_area_print_inner(WINDOW *win, const s_status *status, const int idx) {
	wchar_t tmp[size_inner_get()];

	//
	// Copy the inner part of the line to a temporary variable.
	//
	wmemcpy(tmp, &_data[idx][2], size_inner_get() - 1);

	//
	// Add a terminating \0.
	//
	tmp[ size_inner_get() - 1] = U_TERM;

	log_debug("inner: '%ls'", tmp);

	//
	// If the line is the status line and the game has ended, we use an other
	// color.
	//
	if (idx == IDX_STATUS && s_status_is_end(status)) {
		colors_normal_end_attr(win);
	}

	mvwaddwstr(win, _pos.row + IDX_STATUS, _pos.col + 2, tmp);
}

/******************************************************************************
 * The function prints the info area at the absolute position.
 *****************************************************************************/

void info_area_print(WINDOW *win, const s_status *status) {

	log_debug("row: %d col: %d", _pos.row, _pos.col);

	colors_normal_set_attr(win, CLR_NONE);

	//
	// Print each line
	//
	for (int i = 0; i < L_ROWS; i++) {
		mvwaddwstr(win, _pos.row + i, _pos.col, _data[i]);
	}

	//
	// If the game has ended, we print the inner status line again, which is
	// formated in a different way.
	//
	if (s_status_is_end(status)) {
		info_area_print_inner(win, status, IDX_STATUS);
	}
}

/******************************************************************************
 * The function sets the inner status line in case of the end of the game.
 *****************************************************************************/

void info_area_set_end(WINDOW *win, const s_status *status) {

	//
	// It is assumed that the game is already marked as ended.
	//
	if (!s_status_is_end(status)) {
		log_exit_str("Wrong status!");
	}

	//
	// Set the inner status line
	//
	fmt_center(&_data[IDX_STATUS][2], size_inner_get(), U_EMPTY, FMT_END);
	add_border(_data[IDX_STATUS], size_line_get(), U_VLINE, U_EMPTY);

	//
	// Print the inner line.
	//
	info_area_print_inner(win, status, IDX_STATUS);
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
	if (pixel->row >= _pos.row + L_ROWS || pixel->col >= _pos.col + L_COLS - 1) {
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

	const wchar_t c = _data[row][col] == L'\0' ? L' ' : _data[row][col];

	colors_normal_set_attr(win, color);

	mvwprintw(win, pixel->row, pixel->col, "%lc", c);
}
