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

#include <ncurses.h>

#include "blocks.h"

/******************************************************************************
 * The definition of color indices. They are used as an index for an array with
 * color pairs.
 *****************************************************************************/

#define CLR_NONE        0

#define CLR_RED_        1
#define CLR_GREE        2
#define CLR_BLUE        3
#define CLR_YELL        4

#define CLR_GREY_DARK_  5
#define CLR_GREY_MID__  6
#define CLR_GREY_LIGHT  7

#define CLR_MK_L        8
#define CLR_MK_N        9

/******************************************************************************
 * The enum defines the different forms of chess pattern.
 *****************************************************************************/

typedef enum e_chess_type {

	//
	// Simple chess pattern with two light colors
	//
	CHESS_SIMPLE_LIGHT,

		//
		// Simple chess pattern with two dark colors
		//
		CHESS_SIMPLE_DARK,

		//
		// Double chess pattern with three colors
		//
		CHESS_DOUBLE
} e_chess_type;

/******************************************************************************
 * Functions and macros
 *****************************************************************************/

#define colors_is_even(r,c) ((r) % 2) == ((c) % 2)

void colors_init();

void colors_init_random(t_block **blocks, const int rows, const int cols);

void colors_normal_set_attr(WINDOW *win, const t_block da_color);

void colors_normal_end_attr(WINDOW *win);

wchar_t colors_chess_attr_char(WINDOW *win, const t_block ga_color, const t_block da_color, const s_point *idx, const e_chess_type chess_type);

#endif /* INC_COLORS_H_ */
