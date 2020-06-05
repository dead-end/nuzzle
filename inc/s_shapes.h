/*
 * MIT License
 *
 * Copyright (c) 2020 dead-end
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

#ifndef INC_S_SHAPES_H_
#define INC_S_SHAPES_H_

#include "blocks.h"

/*******************************************************************************
 * Definition of constants.
 ******************************************************************************/

#define SHAPE_DIM 5

#define SHAPE_DEF 1

#define SHAPE_UNDEF 0

/*******************************************************************************
 * The definition of the shape structure.
 ******************************************************************************/

typedef struct s_shape {

	//
	// Currently the label contains the number of blocks that are set. This can
	// be used to modify the probabilities of the random selection.
	//
	// TODO: currently not in use
	int label;

	//
	// The array of the blocks.
	//
	short blocks[SHAPE_DIM][SHAPE_DIM];

} s_shape;

/*******************************************************************************
 * Definition of functions.
 ******************************************************************************/

void s_shapes_read(const char *path);

void s_shapes_init_random(t_block **blocks, const int rows, const int cols);

#endif /* INC_S_SHAPES_H_ */
