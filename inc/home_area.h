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

#ifndef INC_HOME_AREA_H_
#define INC_HOME_AREA_H_

#include <ncurses.h>

#include "s_area.h"
#include "s_status.h"

int home_area_get_idx(const s_point *pixel);

bool home_area_can_drop_anywhere(s_area *area);

void home_area_mark_drop();

bool home_area_refill(const s_game_cfg *game_cfg, const bool force);

bool home_area_pickup(s_area *area, const s_point *pixel);

void home_area_undo_pickup();

void home_area_print(WINDOW *win, const s_status *status);

void home_area_print_pixel(WINDOW *win, const s_status *status, const s_point *pixel, const t_block da_color);

void home_area_reset(const s_game_cfg *game_cfg);

void home_area_create_game(const s_game_cfg *game_cfg);

void home_area_free_game();

s_point home_area_get_size(const bool horizontal);

void home_area_layout(const s_point *pos, const bool horizontal);

bool home_area_next_unused(s_point *pos);

#endif /* INC_HOME_AREA_H_ */
