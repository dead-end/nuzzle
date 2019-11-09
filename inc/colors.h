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

#ifndef INC_COLORS_H_
#define INC_COLORS_H_

// TODO: colors_get_random() => remove include
#include "blocks.h"

/******************************************************************************
 *
 *****************************************************************************/

enum e_colors {
	color_none = 0, color_red = 1, color_green = 2, color_blue = 3, color_yellow = 4,
};

#define CP_DEFAULT 0

//
// Used by: attrset(COLOR_PAIR(area->blocks[block.row][block.col]));
//
// TODO: rename
#define CP_RED_BLACK 1
#define CP_GREEN_BLACK 2
#define CP_BLUE_BLACK 3
#define CP_YELLOW_BLACK 4

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

void colors_info_area_attr(const enum e_colors color);

void colors_bg_attr(const enum e_colors color);

void colors_game_attr(const enum e_colors fg, const enum e_colors bg, const bool even);

// TODO: colors_get_random() => remove include
void colors_init_random(t_block **blocks, const int rows, const int cols);
// TODO: name _empty
#define colors_get_color() (rand() % 4) + 1

#endif /* INC_COLORS_H_ */
