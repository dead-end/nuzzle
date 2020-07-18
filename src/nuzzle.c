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
#include <linux/limits.h>

#include "s_game_cfg.h"

#include "s_status.h"

#include "nuzzle.h"
#include "nz_curses.h"
#include "info_area.h"
#include "home_area.h"
#include "bg_area.h"
#include "game.h"
#include "win_menu.h"
#include "file_system.h"

static s_status _status = { .game_cfg = NULL };

/******************************************************************************
 * The exit callback function resets the terminal and frees the memory. This is
 * important if the program terminates after an error.
 *****************************************************************************/

static void exit_callback() {

	game_free_game(&_status);

	//
	// Free the allocated memory.
	//
	game_free();

	//
	// Finish ncurses
	//
	nzc_finish_curses();

	log_debug_str("Exit callback finished!");
}

/******************************************************************************
 * The method initializes the application.
 *****************************************************************************/

static void init() {

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
	// Register exit callback.
	//
	if (on_exit(exit_callback, NULL) != 0) {
		log_exit_str("Unable to register exit function!");
	}

	//
	// Initialize the application stuff.
	//
	colors_init();

	game_init();

	//
	// Read the configuration file.
	//
	s_game_cfg_read(NUZZLE_CFG_FILE);
}

/******************************************************************************
 * The function starts a new game based on the game configuration.
 *****************************************************************************/

static void create_game(s_status *status, const bool free, const s_game_cfg *game_cfg) {

	//
	// If a game is running we have to cleanup up front.
	//
	if (free) {
		game_free_game(status);
	}

	//
	// Initialize the game status
	//
	s_status_init(status, game_cfg);

	//
	// Create the game and the drop area based on the dimensions.
	//
	game_create_game(status);
}

/******************************************************************************
 * The function shows the menu. This can be a start menu or a menu to change a
 * running game. In the second situation, a game is running, that can be
 * continued. So the menu is mostly identical. If the game is running, there is
 * a "continue" choice.
 *****************************************************************************/

void show_menu(s_status *status, const bool show_continue) {
	log_debug_str("Showing start menu");

	//
	// If a game is running, we have to clear the window.
	//
	if (show_continue) {
		clear();
		refresh();
	}

	const int offset = show_continue ? 1 : 0;
	const int idx_exit = offset + s_game_cfg_num;

	//
	// Initialize the choices array.
	//
	const char *choices[offset + s_game_cfg_num + 2];

	//
	// If it is possible to continue the game, we have to add the choice.
	//
	if (show_continue) {
		choices[0] = STR_CONT;
	}

	//
	// Set the titles for the games.
	//
	for (int i = 0; i < s_game_cfg_num; i++) {
		choices[i + offset] = s_game_cfg_get(i)->title;
	}

	//
	// Set the "exit" choice and terminate the array with NULL.
	//
	choices[idx_exit] = STR_EXIT;
	choices[idx_exit + 1] = NULL;

	//
	// Process the menu (the second parameter is a flag to ignore ESC)
	//
	const int idx = wm_process_menu(choices, 0, !show_continue);

	//
	// Continue the current game
	//
	if (show_continue && (idx == 0 || idx == ESC_RETURN)) {
		game_do_center(status);
	}

	//
	// Create a new game
	//
	else if (offset <= idx && idx < offset + s_game_cfg_num) {

		log_debug("TYPE idx: %d", idx - offset);
		create_game(status, show_continue, s_game_cfg_get(idx - offset));

		if (show_continue) {
			game_reset(status);
			game_do_center(status);
		}
	}

	//
	// Exit the game
	//
	else if (idx == idx_exit) {
		exit(0);

	} else {
		log_exit("Unknown index: %d", idx);
	}
}

/******************************************************************************
 * The function does the processing of the mouse event.
 *****************************************************************************/

static void process_mouse_event(s_status *status) {
	MEVENT event;

	if (getmouse(&event) != OK) {

		//
		// Report the details of the mouse event.
		//
		log_exit("Unable to get mouse event! mask: %s coords: %d/%d/%d", bool_str(event.bstate == 0), event.x, event.y, event.z);
		return;
	}

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

	log_debug_str("Starting nuzzle...");

	init();

	show_menu(&_status, false);

	//
	// Without the refresh() the centered window will not be printed.
	//
	refresh();

	game_do_center(&_status);

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

			game_do_center(&_status);

		} else if (c == KEY_ESC || c == 'm') {

			show_menu(&_status, true);

		} else {

			if (s_status_is_end(&_status)) {
				log_debug_str("Ignoring key event due to current game end!");
				continue;
			}

			switch (c) {

			case KEY_MOUSE:
				process_mouse_event(&_status);
				break;

			case KEY_UP:
				game_event_keyboard_mv(&_status, -1, 0);
				break;

			case KEY_DOWN:
				game_event_keyboard_mv(&_status, 1, 0);
				break;

			case KEY_LEFT:
				game_event_keyboard_mv(&_status, 0, -1);
				break;

			case KEY_RIGHT:
				game_event_keyboard_mv(&_status, 0, 1);
				break;

			case '\t':
				game_event_next_home_area(&_status);
				break;

			case 10:

				//				//
				//				// Try to drop the drop area on the game area. If the drop area
				//				// was dropped, the home areas are refilled if necessary.
				//				//
				//				if (game_event_drop(&_status)) {
				//
				//					//
				//					// If the drop area was dropped, then nothing is picked up.
				//					// In this situation game_event_toggle_pickup() will pickup
				//					// the next unused home area.
				//					//
				//					game_event_next_home_area(&_status);
				//				}

				game_event_drop(&_status);
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
