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
#include <menu.h>

#include "common.h"

 /******************************************************************************
   * The function initializes the ncurses mouse support.
   *****************************************************************************/

static void nzc_init_mouse() {

	if (has_mouse()) {
		log_exit_str("Terminal does not support a mouse!");
	}

	//
	// Register mouse events (which do not have a propper error handling)
	//
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION | BUTTON1_PRESSED | BUTTON2_RELEASED | BUTTON3_RELEASED, NULL);

	printf("\033[?1003h\n");

	mouseinterval(0);
}

/******************************************************************************
 * The function finishes the ncurses mouse support.
 *****************************************************************************/

static void nzc_finish_mouse() {

	//
	// Disable mouse movement events, as l = low
	//
	printf("\033[?1003l\n");
}

/******************************************************************************
 * The function initializes the main features of ncurses .
 *****************************************************************************/

void nzc_init_curses() {

	//
	// Initialize screen.
	//
	if (initscr() == NULL) {
		log_exit_str("Unable to initialize the screen.");
	}

	//
	// Disable line buffering.
	//
	if (raw() == ERR) {
		log_exit_str("Unable to set raw mode.");
	}

	//
	// Disable echoing.
	//
	if (noecho() == ERR) {
		log_exit_str("Unable to switch off echoing.");
	}

	//
	// Enable keypad for the terminal.
	//
	if (keypad(stdscr, TRUE) == ERR) {
		log_exit_str("Unable to enable the keypad of the terminal.");
	}

	//
	// Switch off the cursor.
	//
	if (curs_set(0) == ERR) {
		log_exit_str("Unable to set cursor visibility.");
	}

	//
	// Disable ESC delay. (The man page says: "These functions all return TRUE
	// or FALSE, except as noted." which seems not to be correct.
	//
	set_escdelay(0);

	nzc_init_mouse();
}

/******************************************************************************
 * The function finishes the main features of ncurses .
 *****************************************************************************/

void nzc_finish_curses() {

	nzc_finish_mouse();

	//
	// Reset the terminal.
	//
	if (!isendwin()) {
		log_debug_str("Calling: endwin()");
		endwin();
	}
}

/******************************************************************************
 * The function creates a window with the size of the stdscr.
 *****************************************************************************/

WINDOW *nzc_win_create_fully() {

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

	log_debug("Delete window (is null: %s)", bool_str(win == NULL));

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

	if (row < start_row || row >= start_row + getmaxy(win)) {
		return false;
	}

	const int start_col = getbegx(win);

	if (col < start_col || col >= start_col + getmaxx(win)) {
		return false;
	}

	return true;
}

/******************************************************************************
 * The function returns the index of the current item of a menu.
 *****************************************************************************/

int nzc_menu_cur_item_idx(MENU *menu) {

	//
	// Get the currently selected item of the menu
	//
	const ITEM *item = current_item(menu);
	if (item == NULL) {
		log_exit_str("Unable to get current item!");
	}

	//
	// Get the index of the item
	//
	const int idx = item_index(item);
	if (idx == ERR) {
		log_exit_str("Unable to get current item index!");
	}

	log_debug("Current item: %s with index: %d", item_name(item), idx);

	return idx;
}

/******************************************************************************
 * The function sets the index of the currently selected item in a menu. It is
 * ensured that the index is valid.
 *****************************************************************************/

void nzc_menu_set_cur_item_idx(MENU *menu, const int idx) {

	//
	// Get the number of items in the menu
	//
	const int count = item_count(menu);
	if (count == ERR) {
		log_exit_str("Unable to count items!");
	}

	//
	// Ensure that the index is valid
	//
	if (idx < 0 || count - 1 < idx) {
		log_exit("Item index: %d out of range - count: %d", idx, count);
	}

	//
	// Get the array of the items
	//
	ITEM **items = menu_items(menu);
	if (items == NULL) {
		log_exit_str("Unable to get items from the menu!");
	}

	//
	// Set the item index
	//
	if (set_current_item(menu, items[idx]) != E_OK) {
		log_exit("Unable to set the item index: %d", idx);
	}
}