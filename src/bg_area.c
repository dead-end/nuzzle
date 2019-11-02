#include <ncurses.h>

#include "common.h"
#include "colors.h"

/******************************************************************************
 * The background area has only one function, which prints a pixel with a given
 * color and wide character.
 *****************************************************************************/

void bg_area_print_pixel(const s_point *pixel, const enum e_colors color, const wchar_t chr) {

	colors_bg_attr(color);

	mvprintw(pixel->row, pixel->col, "%lc", chr);

	log_debug("row: %d col: %d color: %d", pixel->row, pixel->col, color);
}
