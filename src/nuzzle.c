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
#include <locale.h>
#include <time.h>

#include "info_area.h"
#include "game_area.h"
#include "bg_area.h"
#include "new_area.h"

/******************************************************************************
 * The exit callback function resets the terminal and frees the memory. This is
 * important if the program terminates after an error.
 *****************************************************************************/

static void exit_callback() {

	//
	// Free the allocated memory.
	//
	game_area_free();

	new_area_free();

	//
	// Disable mouse movement events, as l = low
	//
	printf("\033[?1003l\n");

	endwin();

	log_debug_str("Exit callback finished!");
}

/******************************************************************************
 * The function places the areas of the game in the center of the terminal. It
 * is called after the initialization and on the resizing of the terminal.
 *****************************************************************************/

#define DELIM 4

void do_center() {

	//
	// Get the sizes of the different areas.
	//
	const s_point game_size = game_area_get_size();

	const s_point new_size = new_area_get_size();

	const s_point info_size = info_area_get_size();

	//
	// Compute the size of all areas.
	//
	int total_rows = game_size.row;
	int total_cols = game_size.col + DELIM + max(info_size.col, new_size.col);
	//int total_cols = game_size.col + DELIM + info_size.col;

	//
	// Get the center positions
	//
	const int ul_row = (getmaxy(stdscr) - total_rows) / 2;
	const int ul_col = (getmaxx(stdscr) - total_cols) / 2;

	log_debug("upper left row: %d col: %d", ul_row, ul_col);

	game_area_set_pos(ul_row, ul_col);

	info_area_set_pos(ul_row, ul_col + game_size.col + DELIM);

	new_area_set_pos(ul_row + info_size.row + DELIM, ul_col + game_size.col + DELIM);

	//
	// Delete the old content.
	//
	erase();

	//
	// Print the areas at the updated position
	//
	game_area_print();

	info_area_print();

	new_area_process_blocks(DO_PRINT);

	//
	// Call refresh to show the result
	//
	refresh();
}

/******************************************************************************
 * The main function.
 *****************************************************************************/

int main() {

	log_debug_str("Starting nuzzle...");

	bool pressed = false;

	//
	// Set the locale to allow utf8.
	//
	setlocale(LC_CTYPE, "");

	//
	// Intializes random number generator
	//
	time_t t;
	srand((unsigned) time(&t));

	initscr();

	//
	// Line buffering and echoing disabled.
	//
	raw();
	noecho();

	keypad(stdscr, TRUE);

	//
	// Register mouse events
	//
	mousemask(BUTTON1_RELEASED | BUTTON1_CLICKED | BUTTON1_PRESSED | REPORT_MOUSE_POSITION, NULL);

	printf("\033[?1003h\n");

	mouseinterval(0);

	//
	// Register exit callback.
	//
	if (on_exit(exit_callback, NULL) != 0) {
		log_exit_str("Unable to register exit function!");
	}

	colors_init();

	game_area_init();

	new_area_init();

	new_area_fill();

	info_area_init(0);

	do_center();

	//
	// Move the cursor to the initial position (which prevents flickering) and
	// hide the cursor
	//
	move(0, 0);
	curs_set(0);

	refresh();

	for (;;) {
		int c = wgetch(stdscr);

		//
		// Exit with 'q'
		//
		if (c == 'q') {
			break;
		}

		if (c == ERR) {
			log_debug_str("Nothing happened.");

		} else if (c == KEY_RESIZE) {
			do_center();

		} else if (c == KEY_MOUSE) {
			MEVENT event;

			if (getmouse(&event) != OK) {
				log_exit_str("Unable to get mouse event!");
			}

			//			log_debug("Mouse at row=%d, column=%d bstate=0x%08lx BUTTON: PRESS=%ld CLICK=%ld RELEASE=%ld",
			//
			//			event.y, event.x, event.bstate,
			//
			//			event.bstate & BUTTON1_PRESSED, event.bstate & BUTTON1_CLICKED, event.bstate & BUTTON1_RELEASED);

			if (event.bstate & BUTTON1_RELEASED) {

				//
				// Dropping means that the game area is updated. If the new
				// area is deleted, which means that the foreground is deleted,
				// the background will be visible.
				//
				if (new_area_is_dropped()) {

					//
					// Delete the current none-empty blocks. After calling
					// new_area_fill(), the non-empty blocks may be different.
					//
					new_area_process_blocks(DO_DELETE);

					new_area_fill();

					if (!new_area_can_drop()) {
						log_debug_str("ENDDDDDDDDDDDDD");
					}
				} else {
					new_area_process_blocks(DO_DELETE);
				}

				new_area_process(HOME_ROW, HOME_COL);

				pressed = false;

			} else if ((event.bstate & BUTTON1_PRESSED) || pressed) {

				if (!new_area_same_pos(event.y, event.x)) {

					new_area_process_blocks(DO_DELETE);

					new_area_process(event.y, event.x);
				}

				pressed = true;
			}

		} else {
			log_debug("Pressed key %d (%s)", c, keyname(c));
		}

		//
		// Move the cursor to a save place and do the refreshing. If the cursor
		// is not moved a flickering can occur.
		//
		move(0, 0);
		refresh();
	}

	log_debug_str("Nuzzle finished!");

	return 0;
}
