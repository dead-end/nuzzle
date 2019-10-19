#include <ncurses.h>

#include "common.h"
#include "colors.h"

/******************************************************************************
 * The definition of the colors.
 *****************************************************************************/

#define BG_L_G 17
#define BG_D_G 18

#define FG_RED 19
#define BG_RED 20

#define FG_GRE 21
#define BG_GRE 22

#define FG_BLU 23
#define BG_BLU 24

#define FG_YEL 25
#define BG_YEL 26

/******************************************************************************
 * The definition of the color constants.
 *****************************************************************************/

#define FULL 999
#define LIGH 800
#define DARK 400

/******************************************************************************
 * Simple wrapper that initializes a color pair with error handling.
 *****************************************************************************/

static void colors_init_pair(const short pair, const short fg, const short bg) {

	if (init_pair(pair, fg, bg) != OK) {
		log_exit("Unable to init color pair: %d fg: %d bg: %d", pair, fg, bg);
	}
}

/******************************************************************************
 * Simple wrapper that initializes a color with error handling.
 *****************************************************************************/

static void colors_init_color(const short color, const short red, const short green, const short blue) {

	if (init_color(color, red, green, blue) != OK) {
		log_exit("Unable to init color: %d red: %d green: %d blue: %d", color, red, green, blue);
	}
}

/******************************************************************************
 *
 *****************************************************************************/

void colors_init() {

	//
	// Ensure that the terminal is ok
	//
	if (!has_colors()) {
		log_exit_str("Terminal does not support colors!");
	}

	if (!can_change_color()) {
		log_exit_str("Terminal does not support color changes!");
	}

	//
	// Start color
	//
	if (start_color() != OK) {
		log_exit_str("Unable to init colors!");
	}

	//
	// Initialize colors
	//
	colors_init_color(BG_L_G, 250, 250, 250);
	colors_init_color(BG_D_G, 230, 230, 230);

	colors_init_color(FG_RED, FULL, DARK, DARK);
	colors_init_color(BG_RED, FULL, LIGH, LIGH);

	colors_init_color(FG_GRE, DARK, FULL, DARK);
	colors_init_color(BG_GRE, LIGH, FULL, LIGH);

	colors_init_color(FG_BLU, DARK, DARK, FULL);
	colors_init_color(BG_BLU, LIGH, LIGH, FULL);

	colors_init_color(FG_YEL, FULL, FULL, DARK);
	colors_init_color(BG_YEL, FULL, FULL, LIGH);

	//
	// Initialize color pairs
	//
	colors_init_pair(CP_LGR_LGR, BG_L_G, BG_L_G);
	colors_init_pair(CP_DGR_DGR, BG_D_G, BG_D_G);

	colors_init_pair(CP_RED_BLACK, FG_RED, COLOR_BLACK);
	colors_init_pair(CP_GREEN_BLACK, FG_GRE, COLOR_BLACK);
	colors_init_pair(CP_BLUE_BLACK, FG_BLU, COLOR_BLACK);
	colors_init_pair(CP_YELLOW_BLACK, FG_YEL, COLOR_BLACK);

	//
	//
	//
	colors_init_pair(CP_RED_RED, FG_RED, BG_RED);
	colors_init_pair(CP_RED_GRE, FG_RED, BG_GRE);
	colors_init_pair(CP_RED_BLU, FG_RED, BG_BLU);
	colors_init_pair(CP_RED_YEL, FG_RED, BG_YEL);

	colors_init_pair(CP_GRE_RED, FG_GRE, BG_RED);
	colors_init_pair(CP_GRE_GRE, FG_GRE, BG_GRE);
	colors_init_pair(CP_GRE_BLU, FG_GRE, BG_BLU);
	colors_init_pair(CP_GRE_YEL, FG_GRE, BG_YEL);

	colors_init_pair(CP_BLU_RED, FG_BLU, BG_RED);
	colors_init_pair(CP_BLU_GRE, FG_BLU, BG_GRE);
	colors_init_pair(CP_BLU_BLU, FG_BLU, BG_BLU);
	colors_init_pair(CP_BLU_YEL, FG_BLU, BG_YEL);

	colors_init_pair(CP_YEL_RED, FG_YEL, BG_RED);
	colors_init_pair(CP_YEL_GRE, FG_YEL, BG_GRE);
	colors_init_pair(CP_YEL_BLU, FG_YEL, BG_BLU);
	colors_init_pair(CP_YEL_YEL, FG_YEL, BG_YEL);
}

short colors_get_pair(const enum e_colors fg, const enum e_colors bg) {
	short result = 0;

	if (fg == color_none) {
		result = 10 + bg;

	} else if (bg == color_none) {
		result = fg * 10 + 1;

	} else {
		result = fg * 10 + bg;
	}

	log_debug("fg: %d bg: %d pair: %d", fg, bg, result);

	return result;
}
