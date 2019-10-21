#include <ncurses.h>
#include <locale.h>
#include <time.h>

#include "common.h"
#include "s_matrix.h"

#include "colors.h"

#define GAME_SIZE 11

#define ADD_SIZE 3

s_point abs_home = { .row = 10, .col = 60 };

s_point abs_old = { .row = 10, .col = 60 };

#define BLOCK_FULL L'\u2588'

#define BLOCK_EMPTY L' '

//#define BLOCK_BOTH L'\u2593'
#define BLOCK_BOTH L'\u2592'

// TODO: ???
typedef struct s_min_max {

	int min_row;

	int min_col;

	int max_row;

	int max_col;

} s_min_max;

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

void add_matrix_fill(s_area *matrix) {
	int count = 0;

	log_debug_str("Start");

	for (int r = 0; r < matrix->blk_rows; r++) {
		for (int c = 0; c < matrix->blk_cols; c++) {

			//
			// First check if the color is set
			//
			if (rand() % 100 < 60) {
				matrix->block[r][c] = color_none;

			} else {
				matrix->block[r][c] = (rand() % 4) + 1;
				count++;
			}

			log_debug("row: %d col: %d color: %d", r,c,matrix->block[r][c]);
		}
	}

	if (count == 0) {
		add_matrix_fill(matrix);
	}
}

#define game_empty_color(r,c) ((r) % 2 == (c) % 2) ? COLOR_PAIR(CP_LGR_LGR) : COLOR_PAIR(CP_DGR_DGR)

void add_matrix_print_old(const int offset_row, const int offset_col, const s_area *add_matrix) {

	int row, col;

	log_debug_str("Start");

	for (int r = 0; r < add_matrix->blk_rows; r++) {
		for (int c = 0; c < add_matrix->blk_cols; c++) {

			if (add_matrix->block[r][c] != color_none) {

				row = offset_row + r * add_matrix->size_row;
				col = offset_col + c * add_matrix->size_col;

				block_print(row, col, add_matrix->size_row, add_matrix->size_col, COLOR_PAIR(add_matrix->block[r][c]), BLOCK_FULL);

				log_debug("row: %d col: %d color: %d" ,row, col, add_matrix->block[r][c]);
			}
		}
	}
}

void add_matrix_print_old2(const int offset_row, const int offset_col, const s_area *add_matrix) {

	int row, col;

	log_debug_str("Start");

	for (int r = 0; r < add_matrix->blk_rows; r++) {
		for (int c = 0; c < add_matrix->blk_cols; c++) {

			if (add_matrix->block[r][c] != color_none) {

				row = offset_row + r * add_matrix->size_row;
				col = offset_col + c * add_matrix->size_col;

				attrset(COLOR_PAIR(add_matrix->block[r][c]));

				for (int r = 0; r < add_matrix->size_row; r++) {
					for (int c = 0; c < add_matrix->size_col; c++) {
						mvprintw(row + r, col + c, "%lc", BLOCK_FULL);
					}
				}

				log_debug("row: %d col: %d color: %d" ,row, col, add_matrix->block[r][c]);
			}
		}
	}
}

static short game_get_color_pair(const s_area *matrix, const s_point *idx, const enum e_colors fg) {

#ifdef DEBUG
	if (idx->row >= matrix->blk_rows || idx->col >= matrix->blk_cols) {
		log_exit("Index out of range row: %d col: %d", idx->row, idx->col);
	}
#endif

	int color = matrix->block[idx->row][idx->col];

	if (color == color_none && fg == color_none) {
		return (idx->row % 2) == (idx->col % 2) ? CP_LGR_LGR : CP_DGR_DGR;
	}

	return colors_get_pair(fg, color);
}

void add_matrix_print(const int offset_row, const int offset_col, const s_area *game_matrix, const s_area *add_matrix) {

	s_point game_idx;
	s_point abs_pos_pix;
	int offset_block_row, offset_block_col;
	wchar_t chr;

	log_debug_str("Start");

	for (int ir = 0; ir < add_matrix->blk_rows; ir++) {
		for (int ic = 0; ic < add_matrix->blk_cols; ic++) {

			if (add_matrix->block[ir][ic] != color_none) {

				offset_block_row = offset_row + ir * add_matrix->size_row;
				offset_block_col = offset_col + ic * add_matrix->size_col;

				for (int r = 0; r < add_matrix->size_row; r++) {
					for (int c = 0; c < add_matrix->size_col; c++) {

						abs_pos_pix.row = offset_block_row + r;
						abs_pos_pix.col = offset_block_col + c;

						if (s_area_contains(game_matrix, &abs_pos_pix)) {

							s_area_get_index(game_matrix, &abs_pos_pix, &game_idx);

							short color_pair = game_get_color_pair(game_matrix, &game_idx, add_matrix->block[ir][ic]);

							attrset(COLOR_PAIR(color_pair));

							if (add_matrix->block[ir][ic] != color_none) {
								if (game_matrix->block[game_idx.row][game_idx.col] != color_none) {
									chr = BLOCK_BOTH;
								} else {
									chr = BLOCK_FULL;
								}
							} else {
								chr = BLOCK_EMPTY;
							}

							mvprintw(abs_pos_pix.row, abs_pos_pix.col, "%lc", chr);

							log_debug("inside row: %d col: %d color: %d", offset_block_row, offset_block_col, add_matrix->block[ir][ic]);

						} else {

							attrset(COLOR_PAIR(add_matrix->block[ir][ic]));
							mvprintw(abs_pos_pix.row, abs_pos_pix.col, "%lc", BLOCK_FULL);

							log_debug("outside row: %d col: %d color: %d", abs_pos_pix.row, abs_pos_pix.col, add_matrix->block[ir][ic]);
						}
					}
				}
			}
		}
	}
}

short game_get_color_pair_old(const s_area *matrix, const s_point *idx) {

#ifdef DEBUG
	if (idx->row >= matrix->blk_rows || idx->col >= matrix->blk_cols) {
		log_exit("Index out of range row: %d col: %d", idx->row, idx->col);
	}
#endif

	int color = matrix->block[idx->row][idx->col];

	switch (color) {

	case color_none:
		return (idx->row % 2) == (idx->col % 2) ? CP_LGR_LGR : CP_DGR_DGR;

	case color_red:
		return CP_RED_BLACK;

	case color_green:
		return CP_GREEN_BLACK;

	case color_blue:
		return CP_BLUE_BLACK;

	case color_yellow:
		return CP_YELLOW_BLACK;

	default:
		log_exit("Unknown color: %d in row: %d col: %d", color, idx->row, idx->col)
		;
	}
}

void add_matrix_delete(const int offset_row, const int offset_col, const s_area *game_matrix, const s_area *add_matrix) {
	int abs_block_row, abs_block_col;

	s_point abs_pos;
	s_point index;

	log_debug_str("Start");

	for (int ir = 0; ir < add_matrix->blk_rows; ir++) {
		for (int ic = 0; ic < add_matrix->blk_cols; ic++) {

			if (add_matrix->block[ir][ic] != color_none) {
				abs_block_row = offset_row + ir * add_matrix->size_row;
				abs_block_col = offset_col + ic * add_matrix->size_col;

				for (int r = 0; r < add_matrix->size_row; r++) {
					for (int c = 0; c < add_matrix->size_col; c++) {
						abs_pos.row = abs_block_row + r;
						abs_pos.col = abs_block_col + c;

						if (s_area_contains(game_matrix, &abs_pos)) {

//							s_matrix_get_index(game_matrix, &abs_pos, &index);
//
//							attrset(game_empty_color(index.row, index.col));

							s_area_get_index(game_matrix, &abs_pos, &index);

							short color_pair = game_get_color_pair(game_matrix, &index, color_none);

							attrset(COLOR_PAIR(color_pair));

							mvprintw(abs_pos.row, abs_pos.col, "%lc", BLOCK_EMPTY);
						} else {
							attrset(COLOR_PAIR(0));
							mvprintw(abs_pos.row, abs_pos.col, "%lc", BLOCK_EMPTY);
						}
					}
				}

			}

			//log_debug("row: %d col: %d color: %d" , offset_row + r * (SIZE_ROW), offset_col + c * (SIZE_COL), ha[r][c]);
		}
	}
}

void game_print_empty(const s_area *matrix) {
	s_point point;
	s_point idx;
	short color_pair;

	log_debug_str("start");

	for (int r = 0; r < matrix->blk_rows; r++) {
		for (int c = 0; c < matrix->blk_cols; c++) {

			point.row = matrix->abs_row + matrix->size_row * r;
			point.col = matrix->abs_col + matrix->size_col * c;

			s_area_get_index(matrix, &point, &idx);

			color_pair = game_get_color_pair(matrix, &idx, color_none);

			block_print(point.row, point.col, matrix->size_row, matrix->size_col, COLOR_PAIR(color_pair), BLOCK_EMPTY);
		}
	}

	log_debug_str("end");
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

	s_area_init(game_matrix);

	s_area *add_matrix = s_area_create(ADD_SIZE, ADD_SIZE);
	s_area_set_size(add_matrix, 2, 4);

	add_matrix_fill(game_matrix);
	game_print_empty(game_matrix);

	add_matrix_fill(add_matrix);
	log_debug_str("print add_matrix");
	add_matrix_print(abs_home.row, abs_home.col, game_matrix, add_matrix);

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

				add_matrix_delete(abs_old.row, abs_old.col, game_matrix, add_matrix);
				add_matrix_print(abs_home.row, abs_home.col, game_matrix, add_matrix);

				s_point_set(&abs_old, abs_home.row, abs_home.col);
				pressed = false;

			} else if ((event.bstate & BUTTON1_PRESSED) || pressed) {

				if (event.y != abs_old.row || event.x != abs_old.col) {

					add_matrix_delete(abs_old.row, abs_old.col, game_matrix, add_matrix);
					add_matrix_print(event.y, event.x, game_matrix, add_matrix);

					s_point_set(&abs_old, event.y, event.x);
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
