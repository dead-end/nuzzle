#include <ncurses.h>
#include <locale.h>
#include <s_area.h>
#include <time.h>

#include "common.h"
#include "colors.h"
#include "info_area.h"
#include "game_area.h"
#include "bg_area.h"

#define ADD_SIZE 3

s_point abs_home = { .row = 10, .col = 60 };

s_point abs_old = { .row = 10, .col = 60 };

/******************************************************************************
 *
 *****************************************************************************/

void add_matrix_print(const s_area *new_area) {

	log_debug_str("");

	s_point abs_pos_pix;

	s_point new_block;
	s_point new_block_abs;

	log_debug_str("Start");

	for (new_block.row = 0; new_block.row < new_area->blk_rows; new_block.row++) {
		for (new_block.col = 0; new_block.col < new_area->blk_cols; new_block.col++) {

			if (new_area->blocks[new_block.row][new_block.col] == color_none) {
				continue;
			}

			s_area_abs_block(new_area, &new_block, &new_block_abs);

			//
			// Iterate over the pixels of the new_block
			//
			for (abs_pos_pix.row = new_block_abs.row; abs_pos_pix.row < new_block_abs.row + new_area->size.row; abs_pos_pix.row++) {
				for (abs_pos_pix.col = new_block_abs.col; abs_pos_pix.col < new_block_abs.col + new_area->size.col; abs_pos_pix.col++) {

					if (game_area_contains(&abs_pos_pix)) {
						game_area_print_pixel(&abs_pos_pix, new_area->blocks[new_block.row][new_block.col]);

					} else if (info_area_contains(&abs_pos_pix)) {
						info_area_print_pixel(&abs_pos_pix, new_area->blocks[new_block.row][new_block.col]);

					} else {
						bg_area_print_pixel(&abs_pos_pix, new_area->blocks[new_block.row][new_block.col], BLOCK_FULL);
					}
				}
			}
		}

	}
}

/******************************************************************************
 *
 *****************************************************************************/

void add_matrix_delete(const s_area *new_area) {

	s_point new_block;
	s_point new_block_abs;
	s_point new_block_pixel;

	log_debug_str("Start");

	for (new_block.row = 0; new_block.row < new_area->blk_rows; new_block.row++) {
		for (new_block.col = 0; new_block.col < new_area->blk_cols; new_block.col++) {

			if (new_area->blocks[new_block.row][new_block.col] == color_none) {
				continue;
			}

			//
			// Get the absolute position of the upper left pixel of the current
			// block.
			//
			s_area_abs_block(new_area, &new_block, &new_block_abs);

			//
			// Iterate over the pixels of the new_block
			//
			for (new_block_pixel.row = new_block_abs.row; new_block_pixel.row < new_block_abs.row + new_area->size.row; new_block_pixel.row++) {
				for (new_block_pixel.col = new_block_abs.col; new_block_pixel.col < new_block_abs.col + new_area->size.col; new_block_pixel.col++) {

					if (game_area_contains(&new_block_pixel)) {
						game_area_print_pixel(&new_block_pixel, color_none);

					} else if (info_area_contains(&new_block_pixel)) {
						info_area_print_pixel(&new_block_pixel, color_none);

					} else {
						bg_area_print_pixel(&new_block_pixel, new_area->blocks[new_block.row][new_block.col], BLOCK_EMPTY);
					}
				}
			}
		}
	}
}

/******************************************************************************
 *
 *****************************************************************************/

static void exit_callback() {

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

	s_area *add_matrix = s_area_create(ADD_SIZE, ADD_SIZE);
	s_area_set_abs(add_matrix, abs_home.row, abs_home.col);
	s_area_set_size(add_matrix, 2, 4);

	colors_init_random(add_matrix->blocks, add_matrix->blk_rows, add_matrix->blk_cols);
	log_debug_str("print add_matrix");
	add_matrix_print(add_matrix);

	info_area_init(0);
//	game_area_pos("Start 11");
	info_area_set_pos(2, abs_home.col);
//	game_area_pos("Start 22");
	info_area_print();

	//
	// Move the cursor to the initial position (which prevents flickering) and
	// hide the cursor
	//
	move(0, 0);
	curs_set(0);

	refresh();

//	game_area_pos("Start while");

	for (;;) {
		int c = wgetch(stdscr);

		//
		// Exit with 'q'
		//
		if (c == 'q')
			break;

		char buffer[512];
		size_t max_size = sizeof(buffer);

		if (c == ERR) {
			snprintf(buffer, max_size, "Nothing happened.");

		} else if (c == KEY_MOUSE) {
			MEVENT event;

			if (getmouse(&event) != OK) {
				log_exit_str("Unable to get mouse event!");
			}

			snprintf(buffer, max_size, "Mouse at row=%d, column=%d bstate=0x%08lx BUTTON: PRESS=%ld CLICK=%ld RELEASE=%ld",

			event.y, event.x, event.bstate,

			event.bstate & BUTTON1_PRESSED, event.bstate & BUTTON1_CLICKED, event.bstate & BUTTON1_RELEASED);

			if (event.bstate & BUTTON1_RELEASED) {
				//	game_area_pos("Start released");

				add_matrix_delete(add_matrix);

				s_area_set_abs(add_matrix, abs_home.row, abs_home.col);
				add_matrix_print(add_matrix);

				pressed = false;

			} else if ((event.bstate & BUTTON1_PRESSED) || pressed) {

				//if (event.y != abs_old.row || event.x != abs_old.col) {
				if (event.y != add_matrix->abs.row || event.x != add_matrix->abs.col) {

					//	game_area_pos("Start pressed");
					add_matrix_delete(add_matrix);

					s_area_set_abs(add_matrix, event.y, event.x);
					add_matrix_print(add_matrix);

				}

				pressed = true;
			}

		} else {
			snprintf(buffer, max_size, "Pressed key %d (%s)", c, keyname(c));
		}

		move(0, 0);
		addstr(buffer);
		clrtoeol();
		move(0, 0);

		refresh();
	}

	game_area_free();

	s_area_free(add_matrix);

	log_debug_str("Nuzzle finished!");

	return 0;
}
