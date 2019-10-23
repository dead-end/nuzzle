#include <ncurses.h>
#include <locale.h>
#include <s_area.h>
#include <time.h>

#include "common.h"
#include "colors.h"

#define GAME_SIZE 11

#define ADD_SIZE 3

s_point abs_home = { .row = 10, .col = 60 };

s_point abs_old = { .row = 10, .col = 60 };

#define BLOCK_FULL L'\u2588'

#define BLOCK_EMPTY L' '

//#define BLOCK_BOTH L'\u2593'
#define BLOCK_BOTH L'\u2592'

/******************************************************************************
 *
 *****************************************************************************/

static void block_print(const int row, const int col, const int size_row, const int size_col, const int attrs, const wchar_t ch) {

	attrset(attrs);

	for (int r = 0; r < size_row; r++) {
		for (int c = 0; c < size_col; c++) {
			mvprintw(row + r, col + c, "%lc", ch);
		}
	}
}

static short game_get_color_pair(const s_area *matrix, const s_point *idx, const enum e_colors fg) {

#ifdef DEBUG
	if (idx->row >= matrix->blk_rows || idx->col >= matrix->blk_cols) {
		log_exit("Index out of range row: %d col: %d", idx->row, idx->col);
	}
#endif

	int color = matrix->blocks[idx->row][idx->col];

	if (color == color_none && fg == color_none) {
		return (idx->row % 2) == (idx->col % 2) ? CP_LGR_LGR : CP_DGR_DGR;
	}

	return colors_get_pair(fg, color);
}

void add_matrix_print(const s_area *game_area, const s_area *new_area) {

	s_point game_idx;
	s_point abs_pos_pix;
	wchar_t chr;

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
			for (abs_pos_pix.row = new_block_abs.row; abs_pos_pix.row < new_block_abs.row + new_area->size_row; abs_pos_pix.row++) {
				for (abs_pos_pix.col = new_block_abs.col; abs_pos_pix.col < new_block_abs.col + new_area->size_col; abs_pos_pix.col++) {

					if (s_area_contains(game_area, &abs_pos_pix)) {

						s_area_get_block(game_area, &abs_pos_pix, &game_idx);

						short color_pair = game_get_color_pair(game_area, &game_idx, new_area->blocks[new_block.row][new_block.col]);

						attrset(COLOR_PAIR(color_pair));

						if (new_area->blocks[new_block.row][new_block.col] != color_none) {
							if (game_area->blocks[game_idx.row][game_idx.col] != color_none) {
								chr = BLOCK_BOTH;
							} else {
								chr = BLOCK_FULL;
							}
						} else {
							chr = BLOCK_EMPTY;
						}

						mvprintw(abs_pos_pix.row, abs_pos_pix.col, "%lc", chr);

						log_debug("inside row: %d col: %d color: %d", offset_block_row, offset_block_col, new_area->blocks[ir][ic]);

					} else {

						attrset(COLOR_PAIR(new_area->blocks[new_block.row][new_block.col]));
						mvprintw(abs_pos_pix.row, abs_pos_pix.col, "%lc", BLOCK_FULL);

						log_debug("outside row: %d col: %d color: %d", abs_pos_pix.row, abs_pos_pix.col, new_area->blocks[ir][ic]);
					}
				}
			}
		}

	}
}

void add_matrix_delete(const s_area *game_area, const s_area *new_area) {

	s_point game_block;

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
			for (new_block_pixel.row = new_block_abs.row; new_block_pixel.row < new_block_abs.row + new_area->size_row; new_block_pixel.row++) {
				for (new_block_pixel.col = new_block_abs.col; new_block_pixel.col < new_block_abs.col + new_area->size_col; new_block_pixel.col++) {

					if (s_area_contains(game_area, &new_block_pixel)) {

						s_area_get_block(game_area, &new_block_pixel, &game_block);

						short color_pair = game_get_color_pair(game_area, &game_block, color_none);

						attrset(COLOR_PAIR(color_pair));

						mvprintw(new_block_pixel.row, new_block_pixel.col, "%lc", BLOCK_EMPTY);
					} else {
						attrset(COLOR_PAIR(0));
						mvprintw(new_block_pixel.row, new_block_pixel.col, "%lc", BLOCK_EMPTY);
					}
				}
			}
		}
	}
}

void game_print_empty(const s_area *area) {
	s_point pixel;
	s_point block;
	short color_pair;

	for (block.row = 0; block.row < area->blk_rows; block.row++) {
		for (block.col = 0; block.col < area->blk_cols; block.col++) {

			//
			// Get the absolute position of the upper left pixel of the current
			// block.
			//
			s_area_abs_block(area, &block, &pixel);

			color_pair = game_get_color_pair(area, &block, color_none);

			block_print(pixel.row, pixel.col, area->size_row, area->size_col, COLOR_PAIR(color_pair), BLOCK_EMPTY);
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

	s_area *game_matrix = s_area_create(GAME_SIZE, GAME_SIZE);
	s_area_set_abs(game_matrix, 2, 2);
	s_area_set_size(game_matrix, 2, 4);

	s_area_init_null(game_matrix);

	s_area *add_matrix = s_area_create(ADD_SIZE, ADD_SIZE);
	s_area_set_abs(add_matrix, abs_home.row, abs_home.col);
	s_area_set_size(add_matrix, 2, 4);

	s_area_init_random(game_matrix);
	game_print_empty(game_matrix);

	s_area_init_random(add_matrix);
	log_debug_str("print add_matrix");
	add_matrix_print(game_matrix, add_matrix);

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

				add_matrix_delete(game_matrix, add_matrix);

				s_area_set_abs(add_matrix, abs_home.row, abs_home.col);
				add_matrix_print(game_matrix, add_matrix);

				pressed = false;

			} else if ((event.bstate & BUTTON1_PRESSED) || pressed) {

				//if (event.y != abs_old.row || event.x != abs_old.col) {
				if (event.y != add_matrix->abs_row || event.x != add_matrix->abs_col) {

					add_matrix_delete(game_matrix, add_matrix);

					s_area_set_abs(add_matrix, event.y, event.x);
					add_matrix_print(game_matrix, add_matrix);

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

	s_area_free(game_matrix);

	s_area_free(add_matrix);

	log_debug_str("Nuzzle finished!");

	return 0;
}
