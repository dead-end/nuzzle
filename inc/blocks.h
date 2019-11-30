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

#ifndef INC_BLOCKS_H_
#define INC_BLOCKS_H_

#include "common.h"

/******************************************************************************
 * The type of a block, which is the color.
 *****************************************************************************/

typedef char t_block;

/******************************************************************************
 * Functions and macros
 *****************************************************************************/

t_block** blocks_create(const int rows, const int cols);

void blocks_free(t_block **blocks, const int rows);

void blocks_set(t_block **blocks, const s_point *dim, const t_block value);

s_point blocks_get_size(const s_point *dim, const s_point *size);

#define block_upper_left(pos,size,idx) (pos) + (size) * (idx)

#endif /* INC_BLOCKS_H_ */
