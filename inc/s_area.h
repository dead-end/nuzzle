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

#include "blocks.h"
#include "common.h"

/******************************************************************************
 * The definition of the area struct.
 *****************************************************************************/

typedef struct s_area {

	//
	// The 2-dimensional array with the block colors.
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

s_point s_area_get_size(const s_area *area);

bool s_area_is_inside(const s_area *area, const int row, const int col);

void s_area_get_block(const s_area *area, const s_point *pixel, s_point *block);

bool s_area_is_aligned(const s_area *area, const int row, const int col);

bool s_area_same_pos(const s_area *area, const int row, const int col);

void s_area_print_block(const s_area *area, const int row, const int col, const wchar_t ch);

void s_area_create(s_area *area, const int dim_row, const int dim_col, const int size_row, const int size_col);

void s_area_free(s_area *area);

/******************************************************************************
 *
 *****************************************************************************/

void s_area_mark_neighbors(const s_area *area, t_block **marks, const int row, const int col, t_block color, int *num);

bool s_area_can_drop_anywhere(s_area *area, s_area *drop_area);

bool s_area_drop(s_area *area, const s_point *idx, const s_area *drop_area, const bool do_drop);

int s_area_remove_blocks(s_area *area, const s_point *idx, const s_area *drop_area, t_block **marks);

void s_area_normalize(s_area *area);

bool s_area_used_area_is_inside(const s_area *area, const s_area *drop_area);

#endif /* INC_S_AREA_H_ */
