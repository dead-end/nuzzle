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

bool s_area_is_inside(const s_area *area, const int row, const int col);

void s_area_get_block(const s_area *area, const s_point *pixel, s_point *block);

bool s_area_is_aligned(const s_area *area, const int row, const int col);

bool s_area_same_pos(const s_area *area, const int row, const int col);

void s_area_print_block(const s_area *area, const int row, const int col, const wchar_t ch);

void s_area_get_offset(const s_area *area, s_point *offset);

#endif /* INC_S_AREA_H_ */
