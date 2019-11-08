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

#ifndef INC_GAME_AREA_H_
#define INC_GAME_AREA_H_

#include <stdbool.h>

#include "colors.h"
#include "common.h"

void game_area_init();

void game_area_free();

bool game_area_contains(const int row, const int col);

void game_area_print_pixel(const s_point *pixel, const enum e_colors color);

s_point game_area_get_size();

void game_area_set_pos(const int row, const int col);

void game_area_print();

bool game_area_is_aligned(const s_point *pixel);

void game_area_get_block(const s_point *pixel, s_point *block);

bool game_area_can_update(const int row, int col, enum e_colors color);

void game_area_do_update(const int row, int col, enum e_colors color);

//t_block game_area_get_color(const int row, const int col);

bool game_area_is_empty(const int row, const int col);

void game_area_set_color(const int row, const int col, const t_block color);

void game_area_mark_neighbors(const int row, const int col, t_block color, int *num);

void game_area_remove_marked();

void game_area_reset_marked();

#endif /* INC_GAME_AREA_H_ */
