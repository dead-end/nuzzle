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

#ifndef INC_S_STATUS_H_
#define INC_S_STATUS_H_

#include "s_game_cfg.h"

/******************************************************************************
 * The structure contains data that represent the status of the game.
 *****************************************************************************/

typedef struct s_status {

	//
	// The flag indicates that the current game is over.
	//
	bool end;

	//
	// The offset is the difference between the position of a mouse event and
	// the upper left corner of a block. For the printing we need the upper
	// left corner.
	//
	// A value of OFFSET_NOT_SET indicates the the drop area is no picked up.
	//
	s_point offset;

	//
	// The configurations of the current game.
	//
	const s_game_cfg *game_cfg;

} s_status;

//
// A negative offset means that the drop area is not picked up.
//
#define OFFSET_NOT_SET -1

#define s_status_is_picked_up(s) ((s)->offset.row != OFFSET_NOT_SET && (s)->offset.col != OFFSET_NOT_SET)

//
// Macros to get and set the end flag.
//
#define s_status_set_end(s) (s)->end = true

#define s_status_is_end(s) ((s)->end)

void s_status_init(s_status *status);

void s_status_release(s_status *status);

void s_status_pickup(s_status *status, const int offset_row, const int offset_col);

void s_status_keyboard_event(s_status *status);

void s_status_update_pos(s_status *status, s_point *pos, const int row, const int col);

#endif /* INC_S_STATUS_H_ */
