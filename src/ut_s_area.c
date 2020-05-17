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
 * The function is the a wrapper, that triggers the internal unit tests.
 *****************************************************************************/

void ut_s_area_exec() {

	test_s_area_copy();

	test_s_area_get_size();

	tets_s_area_get_lr();

	test_s_area_is_inside();

	test_s_area_is_area_inside();
}

