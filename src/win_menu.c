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
#include <menu.h>
#include <string.h>

#include "nz_curses.h"
#include "win_menu.h"
#include "common.h"

const char *headers[] = {

"                     _ ",

" _ __  _   _ _______| | ___ ",

"| '_ \\| | | |_  /_  / |/ _ \\",

"| | | | |_| |/ / / /| |  __/",

"|_| |_|\\__,_/___/___|_|\\___|",

"",

NULL };

/******************************************************************************
 * The function prints the header lines.
 *****************************************************************************/

static void wm_print_header(WINDOW *win) {

	for (int i = 0; headers[i] != NULL; i++) {
		mvwprintw(win, i, 0, "%s", headers[i]);
	}
}

/******************************************************************************
 * The function is called with a NULL terminated array of item labels. For each
 * label an item is created. With the items the menu is created. For the items
 * an array is allocated. To free this array the function menu_items(menu) is
 * called to get a reference to it.
 *****************************************************************************/

static MENU* wm_create_menu(const char **labels) {
	int num_items = 0;

	//
	// Count the number of labels.
	//
	for (const char **ptr = labels; *ptr != NULL; ptr++, num_items++) {
		log_debug("Item label: '%s'", *ptr);
	}

	//
	// Allocate memory for the item array (which is NULL terminated). The items
	// can be determined with menu_items(menu). This is used for freeing the
	// array.
	//
	ITEM **items = xmalloc(sizeof(ITEM*) * (num_items + 1));

	//
	// Set the terminating NULL
	//
	items[num_items] = NULL;

	//
	// Create the items.
	//
	for (int i = 0; i < num_items; i++) {
		log_debug("Creating item: '%s'", labels[i]);

		if ((items[i] = new_item(labels[i], "")) == NULL) {
			log_exit("Unable to create item: '%s'", labels[i]);
		}
	}

	//
	// After the items are created, we can create and return the menu.
	//
	MENU *menu = new_menu(items);
	if (menu == NULL) {
		log_exit_str("Unable to create the menu!");
	}

	return menu;
}

/******************************************************************************
 * The function does an unpost and the freeing of the menu and its items.
 *****************************************************************************/

static void wm_free_menu(MENU *menu) {

	//
	// Ensure that there is a menu.
	//
	if (menu == NULL) {
		return;
	}

	//
	// Get a pointer to the items and the number of items. Calling:
	// free_menu(menu) disconnects the items from the menu, so after that,
	// the data cannot be retrieved.
	//
	ITEM **item_ptr = menu_items(menu);
	if (item_ptr == NULL) {
		log_exit_str("Unable get menu items!");
	}

	//
	// The function returns NULL if the menu is NULL, which was check upfront.
	//
	const int num_items = item_count(menu);

	//
	// Ensure that the menu was posted at all.
	//
	const int result = unpost_menu(menu);
	if (result != E_OK && result != E_NOT_POSTED) {
		log_exit_str("Unable to unpost menu!");
	}

	//
	// The menu has to be freed to be able to free the items. Otherwise the
	// items were connected.
	//
	if (free_menu(menu) != E_OK) {
		log_exit_str("Unable to free menu!");
	}

	//
	// Ensure that the item pointer is not NULL.
	//
	if (item_ptr == NULL) {
		return;
	}

	//
	// Ensure that the form has field.
	//
	if (num_items > 0) {

		//
		// Loop through the item array, which is NULL terminated.
		//
		for (ITEM **ptr = item_ptr; *ptr != NULL; ptr++) {
			log_debug("Freeing item: '%s'", item_name(*ptr));

			if (free_item(*ptr) != E_OK) {
				log_exit_str("Unable to free item!");
			}
		}
	}

	//
	// The last step is to free the items array.
	//
	log_debug_str("Freeing the items array!");
	free(item_ptr);
}

/******************************************************************************
 * The function does the setup of the menu, which is the linking of the menu
 * with its windows.
 *****************************************************************************/

static void wm_menu_link(WINDOW *menu_win, WINDOW *menu_derwin, MENU *menu) {

	//
	// Do not mark the current item.
	//
	if (set_menu_mark(menu, "") != E_OK) {
		log_exit_str("Unable to set menu mark!");
	}

	//
	// Set main window and sub window
	//
	if (set_menu_win(menu, menu_win) != E_OK) {
		log_exit_str("Unable to set menu to the window!");
	}

	if (set_menu_sub(menu, menu_derwin) != E_OK) {
		log_exit_str("Unable to set menu to the window!");
	}

	//
	// Post the menu
	//
	if (post_menu(menu) != E_OK) {
		log_exit_str("Unable to post menu!");
	}
}

/******************************************************************************
 * The function frees the menu and the associated windows.
 *****************************************************************************/

static void wm_free(WINDOW *menu_win, WINDOW *menu_derwin, MENU *menu) {

	log_debug_str("Freeing menu and windows!");

	//
	// Free the menu with its items
	//
	wm_free_menu(menu);

	//
	// Free the windows.
	//
	nzc_win_del(menu_derwin);

	nzc_win_del(menu_win);
}

/******************************************************************************
 * The function creates a window and places it centered on the stdscr.
 *****************************************************************************/

static WINDOW* wm_create_centered_win(const s_point *dim) {
	WINDOW *win;

	//
	// Compute the centered position.
	//
	const int center_row = center(getmaxy(stdscr), dim->row);
	const int center_col = center(getmaxx(stdscr), dim->col);

	log_debug("center: %d/%d", center_row, center_col);

	//
	// Create the windows at the given position.
	//
	win = newwin(dim->row, dim->col, center_row, center_col);
	if (win == NULL) {
		log_exit_str("Unable to create window!");
	}

	return win;
}

/******************************************************************************
 * The function creates a derived window. The column position is centered and
 * the row position is at the tail of the parent window.
 *****************************************************************************/

static WINDOW* wm_create_centered_derwin(WINDOW *win_parent, const s_point *dim) {
	WINDOW *win_derived;

	const int start_row = getmaxy(win_parent) - dim->row;
	const int center_col = center(getmaxx(win_parent), dim->col);

	log_debug("start row: %d center column: %d ", start_row, center_col);

	win_derived = derwin(win_parent, dim->row, dim->col, start_row, center_col);
	if (win_derived == NULL) {
		log_exit_str("Unable to create window!");
	}

	return win_derived;
}

/******************************************************************************
 *
 *****************************************************************************/

static int wm_event_loop(WINDOW *menu_win, MENU *menu, const bool ignore_esc) {
	int c;

	if (keypad(menu_win, TRUE) != OK) {
		log_exit_str("Unable to set key pad!");
	}

	while (true) {

		c = wgetch(menu_win);

		//
		// Quit
		//
		if (c == 'q') {
			exit(EXIT_SUCCESS);
		}

		//
		// On escape return or ignore
		//
		if (c == KEY_ESC) {
			if (!ignore_esc) {
				return ESC_RETURN;
			}
		}

		//
		// On enter return the selected item
		//
		else if (c == KEY_ENTER || c == 10) {
			return nzc_menu_cur_item_idx(menu);
		}

		//
		// Key DOWN
		//
		else if (c == KEY_DOWN) {
			menu_driver(menu, REQ_DOWN_ITEM);
			nzc_win_refresh(menu_win);

		}

		//
		// Key UP
		//
		else if (c == KEY_UP) {
			menu_driver(menu, REQ_UP_ITEM);
			nzc_win_refresh(menu_win);
		}

		//
		// Process mouse events
		//
		else if (c == KEY_MOUSE) {
			MEVENT event;

			if (getmouse(&event) != OK) {
				log_exit_str("Unable to get mouse event!");
			}

			WINDOW *win_sub = menu_sub(menu);
			if (!nzc_win_is_inside(win_sub, event.y, event.x)) {
				continue;
			}

			const int idx = event.y - getbegy(win_sub);

			//
			// Get the index of the currently selected item
			//
			const int cur_idx = nzc_menu_cur_item_idx(menu);

			if (idx != cur_idx) {
				nzc_menu_set_cur_item_idx(menu, idx);
				nzc_win_refresh(menu_win);
			}

			//
			// If the button was pressed
			//
			if (event.bstate & BUTTON1_PRESSED) {
				return idx;
			}
		}
	}
}

/******************************************************************************
 * The function is called from outside with and NULL terminated array of labels
 * for a menu. The function returns the index of the selected menu item. All
 * resources are allocated and freed.
 *****************************************************************************/

int wm_process_menu(const char **labels, const int selected, const bool ignore_esc) {
	MENU *menu;
	WINDOW *menu_win;
	WINDOW *menu_derwin;

	//
	// Get the dimension of the label and add a padding.
	//
	s_point dim_labels = strs_dim(labels);
	dim_labels.col += 4;

	const s_point dim_header = strs_dim(headers);
	const s_point dim_total = { .row = dim_header.row + dim_labels.row, .col = dim_header.col };

	//
	// Create a two dimensional array for the centered labels.
	//
	char label_block[dim_labels.row][dim_labels.col + 1];

	//
	// Create an (NULL terminated) array for the labels.
	//
	const char *label_block_ptr[dim_labels.row + 1];

	for (int i = 0; i < dim_labels.row; i++) {
		label_block_ptr[i] = cpy_str_centered(label_block[i], dim_labels.col + 1, labels[i]);
	}

	label_block_ptr[dim_labels.row] = NULL;

	//
	// Create the windows for the menu.
	//
	menu_win = wm_create_centered_win(&dim_total);
	menu_derwin = wm_create_centered_derwin(menu_win, &dim_labels);

	//
	// Create the menu with an array of item labels.
	//
	menu = wm_create_menu(label_block_ptr);

	wm_menu_link(menu_win, menu_derwin, menu);

	nzc_menu_set_cur_item_idx(menu, selected);

	wm_print_header(menu_win);

	nzc_win_refresh(menu_win);

	const int idx = wm_event_loop(menu_win, menu, ignore_esc);

	wm_free(menu_win, menu_derwin, menu);

	return idx;
}
