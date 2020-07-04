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

#ifndef INC_S_AREA_H_
#define INC_S_AREA_H_

#include <ncurses.h>

#include "blocks.h"
#include "colors.h"

/******************************************************************************
 * The definition of the area struct.
 *****************************************************************************/

typedef struct s_area {

	//
	// The 2-dimensional array with the block colors. The colors are defined
	// with the definitions:
	//
	// CLR_NONE, CLR_RED, CLR_GREEN, CLR_BLUE, CLR_YELLOW
	//
	t_block **blocks;

	//
	// The dimensions of the 2-dimensional array.
	//
	s_point dim;

	//
	// The current position (upper left corner) of the blocks.
	//
	s_point pos;

	//
	// The size of a single block.
	//
	s_point size;

} s_area;

/******************************************************************************
 * The functions for the s_area.
 *****************************************************************************/

void s_area_copy(const s_area *from, s_area *to);

void s_area_copy_deep(const s_area *from, s_area *to);

s_point s_area_get_size(const s_area *area);

s_point s_area_get_lr(const s_area *area);

s_point s_area_get_ul(const s_area *area, const s_point *idx);

bool s_area_is_inside(const s_area *area, const s_point *point);

bool s_area_is_area_inside(const s_area *area, const s_area *drop_area);

bool s_area_is_aligned(const s_area *area, const int row, const int col);

bool s_area_align_point(const s_area *area, s_point *point);

s_point s_area_get_max_inner_pos(const s_area *outer_area, const s_area *inner_area);

bool s_area_move_inner_area(const s_area *outer_area, const s_area *inner_area, s_point *point, const s_point *diff);

void s_area_get_block(const s_area *area, const s_point *pixel, s_point *block);

void s_area_set_blocks(const s_area *area, const t_block value);

bool s_area_same_pos(const s_area *area, const int row, const int col);

void s_area_create(s_area *area, const s_point *dim, const s_point *size);

void s_area_free(s_area *area);

void s_area_get_eff_ul(const s_area *area, s_point *ul);

void s_area_normalize(s_area *area);

bool s_area_can_drop_anywhere(s_area *area, s_area *drop_area, s_point *idx);

bool s_area_drop(s_area *area, const s_point *idx, const s_area *drop_area, const bool do_drop);

void s_area_print_block(WINDOW *win, const s_area *area, const s_point *idx, const wchar_t ch);

void s_area_print_chess(WINDOW *win, const s_area *area, const e_chess_type chess_type);

void s_area_print_chess_pixel(WINDOW *win, const s_area *area, const s_point *pixel, const t_block da_color, const e_chess_type chess_type);

#endif /* INC_S_AREA_H_ */
