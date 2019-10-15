#include <ncurses.h>
#include <locale.h>

#include "common.h"

#define OFFSET 2

#define OFFSET_ROW OFFSET

#define OFFSET_COL 2 * OFFSET

#define SIZE_ROW 2

#define SIZE_COL 4

#define BLOCK_FULL L'\u2588'

/******************************************************************************
 * The function prints a block at a position with some attributes.
 *****************************************************************************/

static void print_block(const int row, const int col, int attrs) {

	int abs_row_start = OFFSET_ROW + SIZE_ROW * row;
	int abs_col_start = OFFSET_COL + SIZE_COL * col;

	attrset(attrs);

	for (int r = 0; r < 2; r++) {
		for (int c = 0; c < 4; c++) {
			mvprintw(abs_row_start + r, abs_col_start + c, "%lc", BLOCK_FULL);
		}
	}
}

/******************************************************************************
 * The main function.
 *****************************************************************************/

int main() {

	log_debug_str("Starting nuzzle...");

	//
	// Set the locale to allow utf8.
	//
	setlocale(LC_CTYPE, "");

	initscr();

	//
	// Init colors
	//
	if (!has_colors()) {
		log_exit_str("Terminal does not support colors!");
	}

	start_color();

	init_pair(1, COLOR_YELLOW, COLOR_BLUE);
	init_pair(2, COLOR_BLUE, COLOR_YELLOW);

	//
	// Line buffering and echoing disabled.
	//
	raw();
	noecho();

	for (int y = 0; y < 11; y++) {
		for (int x = 0; x < 11; x++) {

			if (y % 2 == x % 2) {
				print_block(y, x, COLOR_PAIR(1));
			} else {
				print_block(y, x, COLOR_PAIR(2));
			}
		}
	}

	//
	// Move the cursor to the initial position (which prevents flickering) and
	// hide the cursor
	//
	move(0, 0);
	curs_set(0);

	refresh();

	getch();

	endwin();

	log_debug_str("Nuzzle finished!");

	return 0;
}
