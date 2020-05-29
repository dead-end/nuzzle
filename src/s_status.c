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

#include "common.h"
#include "s_status.h"

/******************************************************************************
 * The function initializes the s_status struct.
 *****************************************************************************/

void s_status_init(s_status *status) {

	//
	// A negative offset means that the drop area is not picked up.
	//
	s_point_set(&status->offset, OFFSET_NOT_SET, OFFSET_NOT_SET);

	status->end = false;

	status->pick_up_toggle = false;
}

/******************************************************************************
 * The function sets the status to "NOT picked up" or released.
 *****************************************************************************/

void s_status_release(s_status *status) {

	s_point_set(&status->offset, OFFSET_NOT_SET, OFFSET_NOT_SET);

	status->pick_up_toggle = false;
}

/******************************************************************************
 * The function sets the status to "picked up".
 *****************************************************************************/

void s_status_pickup(s_status *status, const int offset_row, const int offset_col) {

#ifdef DEBUG

	//
	// The offset is not allowed to be negative. This is interpreted as not
	// been picked up.
	//
	if (offset_row < 0 || offset_col < 0) {
		log_exit("Invalid offset %d/%d", offset_row, offset_col);
	}
#endif

	log_debug("Pickup drop area with offset %d/%d", offset_row, offset_col);

	s_point_set(&status->offset, offset_row, offset_col);

	status->pick_up_toggle = true;
}

/******************************************************************************
 * In case of an keyboard event, we do not use the offset. The offset is used
 * for mouse events and to indicate that an area was picked up.
 *****************************************************************************/

void s_status_keyboard_event(s_status *status) {

	//
	// Ensure that something was picked up. (offset >= 0 => picked up)
	//
	if (s_status_is_picked_up(status)) {
		s_point_set(&status->offset, 0, 0);
	}
}

