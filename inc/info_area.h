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

#ifndef INC_INFO_AREA_H_
#define INC_INFO_AREA_H_

#include "common.h"
#include "colors.h"
#include "nuzzle.h"

void info_area_init(const int hs);

void info_area_add_to_score(WINDOW *win, const int add_2_score);

void info_area_reset_score(WINDOW *win);

void info_area_set_pos(const int row, const int col);

s_point info_area_get_size();

void info_area_print();

bool info_area_contains(const s_point *pixel);

void info_area_print_pixel(WINDOW *win, const s_point *pixel, t_block color);

void info_area_set_msg(WINDOW *win, const char *msg, const s_status *status);

#endif /* INC_INFO_AREA_H_ */
