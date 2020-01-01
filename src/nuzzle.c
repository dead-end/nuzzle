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
#include "bg_area.h"
#include "game.h"
#include "win_menu.h"

/******************************************************************************
 * The function initializes ncurses and the locale, which is necessary for the
 * wide character support of ncurses.
 *****************************************************************************/

static void init_ncurses() {

	//
	// Set the locale to allow utf8.
	//
	if (setlocale(LC_CTYPE, "") == NULL) {
		log_exit_str("Unable to set the locale.");
	}

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
	// Register mouse events (which do not have a propper error handling)
	//
	mousemask(BUTTON1_RELEASED | BUTTON1_CLICKED | BUTTON1_PRESSED | REPORT_MOUSE_POSITION, NULL);

	printf("\033[?1003h\n");

	mouseinterval(0);

	// TODO ERROR
	curs_set(0);
}

/******************************************************************************
 * The exit callback function resets the terminal and frees the memory. This is
 * important if the program terminates after an error.
 *****************************************************************************/

static void exit_callback() {

	//
	// Free the allocated memory.
	//
	game_free();

	//
	// Disable mouse movement events, as l = low
	//
	printf("\033[?1003l\n");

	endwin();

	log_debug_str("Exit callback finished!");
}

/******************************************************************************
 * The main function.
 *****************************************************************************/

int main() {

	log_debug_str("Starting nuzzle...");

	bool pressed = false;

	//
	// Intializes random number generator
	//
	time_t t;
	srand((unsigned) time(&t));

	init_ncurses();

	//
	// Register exit callback.
	//
	if (on_exit(exit_callback, NULL) != 0) {
		log_exit_str("Unable to register exit function!");
	}

	colors_init();

	game_init();

	info_area_init(0);

	//---------------------------

	const char *choices[] = { STR_GAME, STR_EXIT, NULL, };
	int idx = wm_process_menu(choices);

	if (idx == 1) {
		exit(0);
	}

	//wgetch(stdscr);
	// --------------------------

	game_do_center();

	//
	// Move the cursor to the initial position (which prevents flickering) and
	// hide the cursor.
	//
	move(0, 0);

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
			game_do_center();

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

			//			if (event.bstate & BUTTON1_RELEASED) {
			//
			//				game_process_event_release(event.y, event.x);
			//
			//				pressed = false;
			//
			//			} else if ((event.bstate & BUTTON1_PRESSED) || pressed) {
			//
			//				game_process_event_pressed(event.y, event.x);
			//
			//				pressed = true;
			//			}

			if (event.bstate & BUTTON1_PRESSED) {
				pressed = !pressed;
				//				if (!pressed) {
				//					pressed = true;
				//				} else {
				//					pressed = false;
				//				}
			}

			if (pressed) {
				// TODO: only if drop area is picked up.
				game_process_event_pressed(event.y, event.x);
			} else {
				game_process_event_release(event.y, event.x);
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
