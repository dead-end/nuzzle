#include <ncurses.h>
#include <stdio.h>

#include "common.h"
#include "colors.h"
#include "s_area.h"
#include "info_area.h"

/******************************************************************************
 * Define the output data.
 *****************************************************************************/

#define ROWS 3

#define COLS 12

char data[ROWS][COLS + 1];

/******************************************************************************
 * The variables contain the score informations.
 *****************************************************************************/

int high_score = 0;

int cur_score = 0;

/******************************************************************************
 * The struct contains the absolute position of the info area.
 *****************************************************************************/

s_point pos;

/******************************************************************************
 * The function initializes the info area.
 *****************************************************************************/

void info_area_init(const int hs) {
	high_score = hs;

	if (snprintf(data[0], COLS + 1, "nuzzle 0.1.0") >= COLS + 1) {
		log_exit("Truncated: %s", data[0]);
	}

	if (snprintf(data[1], COLS + 1, "high: %6d", high_score) >= COLS + 1) {
		log_exit("Truncated: %s", data[1]);
	}

	if (snprintf(data[2], COLS + 1, "now:  %6d", cur_score) >= COLS + 1) {
		log_exit("Truncated: %s", data[2]);
	}
}

/******************************************************************************
 * The function updates the current score, by adding something. After this the
 * updated info area has to be repreinted.
 *****************************************************************************/

void info_area_add_to_score(const int add_2_score) {

	cur_score += add_2_score;

	if (snprintf(data[2], COLS + 1, "now:  %6d", cur_score) >= COLS + 1) {
		log_exit("Truncated: %s", data[0]);
	}

	info_area_print();
}

/******************************************************************************
 * The function sets the absolute position of the info area. If this is defined
 * as a marco, the s_point struct has to be set in the global scope.
 *****************************************************************************/

void info_area_set_pos(const int row, const int col) {
	pos.row = row;
	pos.col = col;
}

/******************************************************************************
 * The function prints the info area at the absolute position.
 *****************************************************************************/

void info_area_print() {

	attrset(COLOR_PAIR(CP_DEFAULT));

	for (int i = 0; i < ROWS; i++) {
		mvprintw(pos.row + i, pos.col, data[i]);
	}
}

/******************************************************************************
 *
 *****************************************************************************/

bool info_area_contains(const s_point *pixel) {

	if (pixel->row < pos.row || pixel->col < pos.col) {
		return false;
	}

	if (pixel->row >= pos.row + ROWS || pixel->col >= pos.col + COLS) {
		return false;
	}

	return true;
}

void info_area_print_pixel(const s_point *pixel, enum e_colors color) {

	const int row = pixel->row - pos.row;
	const int col = pixel->col - pos.col;

	if (row >= ROWS || col >= COLS) {
		log_exit("Wrong row: %d col: %d", row, col);
	}

	switch (color) {

	case color_none:
		attrset(COLOR_PAIR(CP_DEFAULT));
		break;

	case color_red:
		attrset(A_REVERSE| COLOR_PAIR(CP_RED_BLACK));
		break;

	case color_green:
		attrset(A_REVERSE| COLOR_PAIR(CP_GREEN_BLACK));
		break;

	case color_blue:
		attrset(A_REVERSE| COLOR_PAIR(CP_BLUE_BLACK));
		break;

	case color_yellow:
		attrset(A_REVERSE| COLOR_PAIR(CP_YELLOW_BLACK));
		break;

	default:
		log_exit("Unknown color: %d", color)
		;
	}

	mvprintw(pixel->row, pixel->col, "%c", data[row][col]);
}
