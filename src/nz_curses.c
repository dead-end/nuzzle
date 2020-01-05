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

#include "common.h"

/******************************************************************************
 * The function creates a window with the size of the stdscr.
 *****************************************************************************/

WINDOW* nzc_win_create_fully() {

	WINDOW *win = newwin(getmaxy(stdscr), getmaxx(stdscr), 0, 0);
	if (win == NULL) {
		log_exit_str("Unable to create win!");
	}

	log_debug("new win: %d/%d", getmaxy(win), getmaxx(win));

	return win;
}

/******************************************************************************
 * The function frees a window. It is a simple wrapper with error handling.
 *****************************************************************************/

void nzc_win_del(WINDOW *win) {

	//
	// Ensure that the window is initialized.
	//
	if (win == NULL) {
		return;
	}

	//
	// Free the windows.
	//
	if (delwin(win) != OK) {
		log_exit_str("Unable to delete the window!");
	}
}

/******************************************************************************
 * The function refreshes a window. It is a simple wrapper with error handling.
 *****************************************************************************/

void nzc_win_refresh(WINDOW *win) {

	//
	// Ensure that the window is initialized.
	//
	if (win == NULL) {
		return;
	}

	if (wrefresh(win) == ERR) {
		log_exit_str("Unable to refresh window!");
	}
}

/******************************************************************************
 * The function checks whether a row / column is inside a window or not.
 *****************************************************************************/

bool nzc_win_is_inside(WINDOW *win, const int row, const int col) {

	const int start_row = getbegy(win);

	if (row < start_row || row > start_row + getmaxy(win)) {
		return false;
	}

	const int start_col = getbegx(win);

	if (col < start_col || col > start_col + getmaxx(win)) {
		return false;
	}

	return true;
}

