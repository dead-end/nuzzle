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

#ifndef INC_GAME_H_
#define INC_GAME_H_

#include "common.h"

#define HOME_ROW -1
#define HOME_COL -1

void new_area_init();

void new_area_free();

void new_area_fill();

#define DO_PRINT true

#define DO_DELETE false

void new_area_process_blocks(const bool do_print);

s_point new_area_get_size();

void new_area_set_pos(const int row, const int col);

void game_area_set_pos(const int row, const int col);

s_point get_game_size();

void print_game_area();

// -----------------------------------

void game_process_event_pressed(const int row, const int col);

void game_process_event_release(const int row, const int col);

#endif /* INC_GAME_H_ */
