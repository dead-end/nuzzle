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

#define CLR_NONE       0

#define CLR_RED        1
#define CLR_GREEN      2
#define CLR_BLUE       3
#define CLR_YELLOW     4

#define CLR_GREY_DARK  5
#define CLR_GREY_LIGHT 6

/******************************************************************************
 * Functions and macros
 *****************************************************************************/

#define colors_is_even(r,c) ((r) % 2) == ((c) % 2)

void colors_init();

void colors_init_random(t_block **blocks, const int rows, const int cols);

void colors_info_area_attr(WINDOW *win, const t_block bg_color);

void colors_info_end_attr(WINDOW *win);

void colors_set_game_attr(WINDOW *win, const t_block fg_color, const t_block bg_color, const bool even);

void colors_bg_attr(WINDOW *win, const t_block color);

wchar_t colors_get_char(const t_block fg_color, const t_block bg_color);

#endif /* INC_COLORS_H_ */
