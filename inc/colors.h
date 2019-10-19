#ifndef INC_COLORS_H_
#define INC_COLORS_H_

/******************************************************************************
 *
 *****************************************************************************/

enum e_colors {
	color_none = 0, color_red = 1, color_green = 2, color_blue = 3, color_yellow = 4,
};

#define CP_RED_BLACK 1
#define CP_GREEN_BLACK 2
#define CP_BLUE_BLACK 3
#define CP_YELLOW_BLACK 4

//#define CP_G_LIGHT_BLACK 5
//#define CP_G_DARK_BLACK 6

#define CP_LGR_LGR 5
#define CP_DGR_DGR 6

//
//
//
#define CP_RED_RED 11
#define CP_RED_GRE 12
#define CP_RED_BLU 13
#define CP_RED_YEL 14

#define CP_GRE_RED 21
#define CP_GRE_GRE 22
#define CP_GRE_BLU 23
#define CP_GRE_YEL 24

#define CP_BLU_RED 31
#define CP_BLU_GRE 32
#define CP_BLU_BLU 33
#define CP_BLU_YEL 34

#define CP_YEL_RED 41
#define CP_YEL_GRE 42
#define CP_YEL_BLU 43
#define CP_YEL_YEL 44

void colors_init();

short colors_get_pair(const enum e_colors fg, const enum e_colors bg);

#endif /* INC_COLORS_H_ */
