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
#include "home_area.h"
#include "bg_area.h"
#include "game.h"
#include "win_menu.h"

/******************************************************************************
 * The exit callback function resets the terminal and frees the memory. This is
 * important if the program terminates after an error.
 *****************************************************************************/

static void exit_callback() {

	home_area_free();

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

	home_area_create(2, &(s_point ) { 3, 3 }, &(s_point ) { 2, 4 }, colors_init_random);

	game_init(status);
}

/******************************************************************************
 * The function shows the start menu, which has only two options: "start" or
 * "exit"
 *****************************************************************************/

static void show_start_menu() {
	log_debug_str("Showing start menu");

	const char *choices[] = { STR_GAME, STR_EXIT, NULL, };
	int idx = wm_process_menu(choices, 0, true);

	if (idx == 1) {
		exit(0);
	}
}

/******************************************************************************
 * The function shows the game menu. It allows to exit or to restart the game.
 *****************************************************************************/

static void show_game_menu(s_status *status) {
	log_debug_str("Showing game menu");

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
		game_do_center(status);

	}

	//
	// IDX 1: new game
	//
	else if (idx == 1) {
		game_reset(status);

		game_do_center(status);

	}

	//
	// IDX 2: end
	//
	else if (idx == 2) {
		exit(0);
	}
}

/******************************************************************************
 * The function does the processing of the mouse event.
 *****************************************************************************/

static void process_mouse_event(s_status *status, const int c) {
	MEVENT event;

	if (getmouse(&event) != OK) {
		log_exit_str("Unable to get mouse event!");
	}

	log_debug("Button: %d", c);

	if ((event.bstate & BUTTON2_RELEASED) || (event.bstate & BUTTON3_RELEASED)) {

		if (s_status_is_picked_up(status)) {
			game_process_event_undo_pickup(status);
		}

	} else {
		const s_point event_point = { event.y, event.x };

		if (event.bstate & BUTTON1_PRESSED) {

			if (s_status_is_picked_up(status)) {

				if (home_area_get_idx(&event_point) >= 0) {
					game_process_event_undo_pickup(status);
				} else {
					game_event_drop(status);
				}
			} else {
				game_process_do_pickup(status, &event_point);
			}

		} else {

			if (s_status_is_picked_up(status)) {
				game_event_move(status, &event_point);
			}
		}
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

	game_do_center(&status);

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

			game_do_center(&status);

		} else if (c == KEY_ESC || c == 'm') {

			show_game_menu(&status);

		} else {

			if (s_status_is_end(&status)) {
				log_debug_str("Ignoring key event due to current game end!");
				continue;
			}

			switch (c) {

			case KEY_MOUSE:
				process_mouse_event(&status, c);
				break;

			case KEY_UP:
				game_event_keyboard_mv(&status, -1, 0);
				break;

			case KEY_DOWN:
				game_event_keyboard_mv(&status, 1, 0);
				break;

			case KEY_LEFT:
				game_event_keyboard_mv(&status, 0, -1);
				break;

			case KEY_RIGHT:
				game_event_keyboard_mv(&status, 0, 1);
				break;

			case '\t':
				game_event_toggle_pickup(&status);
				break;

			case 10:
				game_event_drop(&status);
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
