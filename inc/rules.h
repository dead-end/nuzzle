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

#ifndef INC_RULES_H_
#define INC_RULES_H_

#include "s_area.h"

/*******************************************************************************
 * Definitions for the rules.
 ******************************************************************************/

//
// The dimension of a square on the game area for the squares-lines rules.
//
#define RULES_SQUARE_DIM 3

//
// Constant to mark blocks of the area.
//
#define RULES_MARKER 1

/*******************************************************************************
 * Function declarations.
 ******************************************************************************/

void rules_create_game(const s_area *area);

void rules_free_game(const s_area *area);

int rules_remove_lines(const s_area *area);

int rules_remove_squares_lines(const s_area *area);

int rules_remove_neighbors(const s_area *area);

#endif /* INC_RULES_H_ */
