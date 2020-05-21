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
#include <time.h>
#include <locale.h>

#include "nuzzle.h"
#include "nz_curses.h"
#include "info_area.h"
#include "bg_area.h"
#include "game.h"
#include "win_menu.h"

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
	// Disable mouse movement events
	//
	nzc_finish_mouse();

	endwin();

	log_debug_str("Exit callback finished!");
}

/******************************************************************************
 * The method initializes the application.
 *****************************************************************************/

static void init(s_status *status) {

	//
	// Set the locale to allow utf8.
	//
	if (setlocale(LC_CTYPE, "") == NULL) {
		log_exit_str("Unable to set the locale.");
	}

	//
	// Initializes random number generator. The function does not return a
	// value.
	//
	time_t t;
	srand((unsigned) time(&t));

	//
	// Initialize the standard ncurses stuff
	//
	nzc_init_curses();

	//
	// Initialize the mouse support for ncurses.
	//
	nzc_init_mouse();

	//
	// Register exit callback.
	//
	if (on_exit(exit_callback, NULL) != 0) {
		log_exit_str("Unable to register exit function!");
	}

	//
	// Initialize the application stuff.
	//
	colors_init();

	info_area_init();

	game_init(status);
}

/******************************************************************************
 * The function shows the start menu, which has only two options: "start" or
 * "exit"
 *****************************************************************************/

static void show_start_menu() {

	const char *choices[] = { STR_GAME, STR_EXIT, NULL, };
	int idx = wm_process_menu(choices, 0, true);

	if (idx == 1) {
		exit(0);
	}
}

/******************************************************************************
 * The main function.
 *****************************************************************************/

int main() {
	s_status status;

	log_debug_str("Starting nuzzle...");

	init(&status);

	show_start_menu();

	//
	// Without the refresh() the centered window will not be printed.
	//
	refresh();

	game_do_center();

	game_win_refresh();

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
			continue;
		}

		if (c == KEY_RESIZE) {

			//
			// Without the refresh() the centered window will not be printed.
			//
			refresh();

			game_do_center();

		} else if (c == KEY_ESC || c == 'm') {
			log_debug_str("ESC");

			//
			// Delete the old content
			//
			clear();
			refresh();

			//
			// Show the menu
			//
			const char *choices[] = { STR_CONT, STR_GAME, STR_EXIT, NULL, };
			const int idx = wm_process_menu(choices, 0, false);

			//
			// IDX 0: continue
			//
			if (idx == 0 || idx == ESC_RETURN) {
				game_do_center();

			}

			//
			// IDX 1: new game
			//
			else if (idx == 1) {
				game_reset(&status);

				game_do_center();

			}

			//
			// IDX 2: end
			//
			else if (idx == 2) {
				exit(0);
			}

		} else if (c == KEY_MOUSE) {

			if (s_status_is_end(&status)) {
				log_debug_str("Ignoring mouse event due to current game end!");
				continue;
			}

			MEVENT event;

			if (getmouse(&event) != OK) {
				log_exit_str("Unable to get mouse event!");
			}

			log_debug("Button: %d", c);

			if ((event.bstate & BUTTON2_RELEASED) || (event.bstate & BUTTON3_RELEASED)) {
				game_process_event_home(&status);

			} else {

				if (event.bstate & BUTTON1_PRESSED) {
					status.pick_up_toggle = !status.pick_up_toggle;
				}

				if (status.pick_up_toggle) {
					// TODO: only if drop area is picked up.
					game_process_event_pressed(&status, event.y, event.x);
				} else {
					game_process_event_release(&status);
				}
			}

		} else {

			if (s_status_is_end(&status)) {
				log_debug_str("Ignoring key event due to current game end!");
				continue;
			}

			switch (c) {

			case KEY_UP:
				game_process_event_keyboard(&status, -1, 0);
				break;

			case KEY_DOWN:
				game_process_event_keyboard(&status, 1, 0);
				break;

			case KEY_LEFT:
				game_process_event_keyboard(&status, 0, -1);
				break;

			case KEY_RIGHT:
				game_process_event_keyboard(&status, 0, 1);
				break;

			case '\t':
				game_process_event_toggle(&status);
				break;

			case 10:
				game_process_event_release(&status);
				break;

			default:
				log_debug("Pressed key %d (%s)", c, keyname(c));
				continue;
			}
		}

		//
		// Do a refresh
		//
		game_win_refresh();
	}

	log_debug_str("Nuzzle finished!");

	return 0;
}
