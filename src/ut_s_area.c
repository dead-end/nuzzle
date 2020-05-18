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

#include "ut_utils.h"
#include "s_area.h"

/******************************************************************************
 * The function checks the s_area_copy() function.
 *****************************************************************************/

static void test_s_area_copy() {

	s_area from, to;

	s_point_set(&from.dim, 1, 2);
	s_point_set(&from.size, 3, 4);
	s_point_set(&from.pos, 5, 6);

	from.blocks = NULL;

	s_area_copy(&to, &from);

	ut_check_s_point(&to.dim, &(s_point ) { 1, 2 }, "checking dim");
	ut_check_s_point(&to.size, &(s_point ) { 3, 4 }, "checking size");
	ut_check_s_point(&to.pos, &(s_point ) { 5, 6 }, "checking pos");

	if (to.blocks != NULL) {
		log_exit_str("Block is not null!");
	}
}

/******************************************************************************
 * The function checks the size computation of an area, which should be the
 * dimension times the size.
 *****************************************************************************/

static void test_s_area_get_size() {
	s_area area;

	s_point_set(&area.dim, 1, 2);
	s_point_set(&area.size, 3, 4);

	const s_point size = s_area_get_size(&area);

	ut_check_s_point(&size, &(s_point ) { 3, 8 }, "checking size");
}

/******************************************************************************
 * The function checks the computation of the lower right point of the area,
 * which should be the dimension times the size plus the position.
 *****************************************************************************/

static void tets_s_area_get_lr() {
	s_area area;

	s_point_set(&area.dim, 1, 2);
	s_point_set(&area.size, 3, 4);
	s_point_set(&area.pos, 5, 6);

	const s_point lower_right = s_area_get_lr(&area);

	ut_check_s_point(&lower_right, &(s_point ) { 7, 13 }, "checking lower right");
}

/******************************************************************************
 * The function checks the s_area_is_inside() function. We use the same values
 * from the last computations, so we know the lower right point is (8,14)
 *
 * 01234567890123
 * 1
 * 2
 * 3
 * 4
 * 5     OOOOXXXX
 * 6     OOOOXXXX
 * 7     OOOOXXXX
 *****************************************************************************/

static void test_s_area_is_inside() {
	s_area area;
	bool result;

	s_point_set(&area.dim, 1, 2);
	s_point_set(&area.size, 3, 4);
	s_point_set(&area.pos, 5, 6);

	//
	// Outside
	//
	result = s_area_is_inside(&area, 0, 0);
	ut_check_bool(result, false, "0,0");

	result = s_area_is_inside(&area, 0, 6);
	ut_check_bool(result, false, "0,6");

	result = s_area_is_inside(&area, 5, 0);
	ut_check_bool(result, false, "5,0");

	//
	// Upper left
	//
	result = s_area_is_inside(&area, 5, 6);
	ut_check_bool(result, true, "5,6");

	//
	// Lower right
	//
	result = s_area_is_inside(&area, 7, 13);
	ut_check_bool(result, true, "7, 13");

	//
	// Outside
	//
	result = s_area_is_inside(&area, 8, 13);
	ut_check_bool(result, false, "8, 13");

	result = s_area_is_inside(&area, 7, 14);
	ut_check_bool(result, false, "7, 14");

	result = s_area_is_inside(&area, 16, 16);
	ut_check_bool(result, false, "16, 16");
}

/******************************************************************************
 * The function checks the s_area_is_area_inside() function.
 *****************************************************************************/

static void test_s_area_is_area_inside() {
	s_area outer, inner;
	bool result;

	s_point_set(&outer.dim, 2, 2);
	s_point_set(&outer.size, 3, 3);
	s_point_set(&outer.pos, 1, 1);

	s_point_set(&inner.dim, 2, 2);
	s_point_set(&inner.size, 2, 2);

	//
	// Outside
	//
	s_point_set(&inner.pos, 0, 0);
	result = s_area_is_area_inside(&outer, &inner);
	ut_check_bool(result, false, "0 ,0");

	//
	// Same upper left
	//
	s_point_set(&inner.pos, 1, 1);
	result = s_area_is_area_inside(&outer, &inner);
	ut_check_bool(result, true, "1, 1");

}

/******************************************************************************
 * The function checks the s_area_is_aligned() function.
 *
 * 0123456
 * 1AAAbbb
 * 2AAAbbb
 * 3AAAbbb
 * 4cccDDD
 * 5cccDDD
 * 6cccDDD
 *****************************************************************************/

static void test_s_area_is_aligned() {
	s_area area;
	bool result;

	s_point_set(&area.dim, 2, 2);
	s_point_set(&area.size, 3, 3);
	s_point_set(&area.pos, 1, 1);

	result = s_area_is_aligned(&area, 4, 1);
	ut_check_bool(result, true, "4, 1");

	result = s_area_is_aligned(&area, 1, 4);
	ut_check_bool(result, true, "1, 4");

	result = s_area_is_aligned(&area, 5, 5);
	ut_check_bool(result, false, "5, 5");

	result = s_area_is_aligned(&area, 1, 5);
	ut_check_bool(result, false, "1, 5");
}

/******************************************************************************
 * The function checks the test_s_area_align_point() function. It uses the
 * previous definition of the area.
 *****************************************************************************/

static void test_s_area_align_point() {
	s_area area;
	s_point point;
	bool result;

	s_point_set(&area.dim, 2, 2);
	s_point_set(&area.size, 3, 3);
	s_point_set(&area.pos, 1, 1);

	s_point_set(&point, 1, 1);
	result = s_area_align_point(&area, &point);
	ut_check_bool(result, false, "1, 1");
	ut_check_s_point(&point, &(s_point ) { 1, 1 }, "1, 1");

	s_point_set(&point, 2, 3);
	result = s_area_align_point(&area, &point);
	ut_check_bool(result, true, "2,3");
	ut_check_s_point(&point, &(s_point ) { 1, 1 }, "2, 3");

	s_point_set(&point, 6, 5);
	result = s_area_align_point(&area, &point);
	ut_check_bool(result, true, "6, 5");
	ut_check_s_point(&point, &(s_point ) { 4, 4 }, "6, 5");
}

/******************************************************************************
 * The function check the test_s_area_get_max_inner_pos() function. The outer
 * area has capital letters and the inner area has lower letters:
 *
 * 0123456
 * 1AABBCC
 * 2AABBCC
 * 3AAaabb
 * 4DDaabb
 * 5DDccdd
 * 6DDccdd
 *****************************************************************************/

static void test_s_area_get_max_inner_pos() {
	s_area outer, inner;

	s_point_set(&outer.dim, 2, 3);
	s_point_set(&outer.size, 3, 2);
	s_point_set(&outer.pos, 1, 1);

	s_point_set(&inner.dim, 2, 2);
	s_point_set(&inner.size, 2, 2);

	const s_point point = s_area_get_max_inner_pos(&outer, &inner);
	ut_check_s_point(&point, &(s_point ) { 3, 3 }, "max inner pos");
}

/******************************************************************************
 * The function check the test_s_area_get_max_inner_pos() function.
 *
 * 012345678
 * 1xx--xx--
 * 2xx--xx--
 * 3--xx--xx
 * 4--xx--xx
 * 5xx--xx--
 * 6xx--xx--
 * 7--xx--xx
 * 8--xx--xx
 *****************************************************************************/

static void test_s_area_move_inner_area() {
	s_area outer, inner;
	bool result;
	s_point point;

	s_point_set(&outer.dim, 4, 4);
	s_point_set(&outer.size, 2, 2);
	s_point_set(&outer.pos, 1, 1);

	s_point_set(&inner.dim, 2, 2);
	s_point_set(&inner.size, 2, 2);

	//
	// Movements that fail
	//
	s_point_set(&point, 3, 5);
	result = s_area_move_inner_area(&outer, &inner, &point, &(s_point ) { 0, 1 });
	ut_check_bool(result, false, "right - false");
	ut_check_s_point(&point, &(s_point ) { 3, 5 }, "right - false");

	s_point_set(&point, 5, 1);
	result = s_area_move_inner_area(&outer, &inner, &point, &(s_point ) { 1, 0 });
	ut_check_bool(result, false, "down - false");
	ut_check_s_point(&point, &(s_point ) { 5, 1 }, "down - false");

	s_point_set(&point, 3, 1);
	result = s_area_move_inner_area(&outer, &inner, &point, &(s_point ) { 0, -1 });
	ut_check_bool(result, false, "left - false");
	ut_check_s_point(&point, &(s_point ) { 3, 1 }, "left - false");

	s_point_set(&point, 1, 5);
	result = s_area_move_inner_area(&outer, &inner, &point, &(s_point ) { -1, 0 });
	ut_check_bool(result, false, "up - false");
	ut_check_s_point(&point, &(s_point ) { 1, 5 }, "up - false");

	//
	// Movements that work
	//
	s_point_set(&point, 1, 1);
	result = s_area_move_inner_area(&outer, &inner, &point, &(s_point ) { 0, 1 });
	ut_check_bool(result, true, "right - true");
	ut_check_s_point(&point, &(s_point ) { 1, 3 }, "right - true");

	s_point_set(&point, 3, 1);
	result = s_area_move_inner_area(&outer, &inner, &point, &(s_point ) { 1, 0 });
	ut_check_bool(result, true, "down - true");
	ut_check_s_point(&point, &(s_point ) { 5, 1 }, "down - true");

	s_point_set(&point, 3, 3);
	result = s_area_move_inner_area(&outer, &inner, &point, &(s_point ) { 0, -1 });
	ut_check_bool(result, true, "left - true");
	ut_check_s_point(&point, &(s_point ) { 3, 1 }, "left - true");

	s_point_set(&point, 5, 3);
	result = s_area_move_inner_area(&outer, &inner, &point, &(s_point ) { -1, 0 });
	ut_check_bool(result, true, "up - true");
	ut_check_s_point(&point, &(s_point ) { 3, 3 }, "up - true");
}

/******************************************************************************
 * The function is the a wrapper, that triggers the internal unit tests.
 *****************************************************************************/

void ut_s_area_exec() {

	test_s_area_copy();

	test_s_area_get_size();

	tets_s_area_get_lr();

	test_s_area_is_inside();

	test_s_area_is_area_inside();

	test_s_area_is_aligned();

	test_s_area_align_point();

	test_s_area_get_max_inner_pos();

	test_s_area_move_inner_area();
}

