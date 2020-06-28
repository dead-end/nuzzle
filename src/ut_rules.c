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
#include "rules.h"
#include "colors.h"

//
// test_check_lines() requires that row == col
//
static const s_point _dim = { 6, 6 };

static const s_point _size = { 1, 1 };

/******************************************************************************
 * The function ensures that the blocks of the array are all not set.
 *****************************************************************************/

static void check_empty(t_block **blocks, const int row, const int col) {

	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			ut_check_int(blocks[i][j], CLR_NONE, "mark");
		}
	}
}

/******************************************************************************
 * The function checks the marking and removing of squares.
 *****************************************************************************/

static void test_check_squares() {

	//
	// Create and initialize the area
	//
	s_area area;
	s_area_create(&area, &_dim, &_size);
	s_area_set_blocks(&area, 0);

	//
	// Set square at 0/0
	//
	for (int i = 0; i < RULES_SQUARE_DIM; i++) {
		for (int j = 0; j < RULES_SQUARE_DIM; j++) {
			area.blocks[i][j] = RULES_MARKER;
		}
	}

	//
	// Set square at 0/3
	//
	for (int i = 0; i < RULES_SQUARE_DIM; i++) {
		for (int j = RULES_SQUARE_DIM; j < RULES_SQUARE_DIM * 2; j++) {
			area.blocks[i][j] = RULES_MARKER;
		}
	}

	//
	//  Create the rules and remove the lines.
	//
	rules_create_game(&area);

	const int count = rules_remove_squares_lines(&area);

	//
	// Ensure that the result is as expected.
	//
	ut_check_int(count, 18, "removed");

	check_empty(area.blocks, _dim.row, _dim.col);

	//
	// Free the allocated area.
	//
	rules_free_game(&area);

	s_area_free(&area);
}

/******************************************************************************
 * The function checks the marking and removing of vertical and horizontal
 * lines.
 *****************************************************************************/

static void test_check_lines() {

	//
	// Create and initialize the area
	//
	s_area area;
	s_area_create(&area, &_dim, &_size);
	s_area_set_blocks(&area, 0);

	//
	// For this test a symmetric dim is required.
	//
	if (_dim.row != _dim.col) {
		log_exit("Area dim is not symetric: %d/%d", _dim.row, _dim.col);
	}

	//
	// Set a horizontal and vertical line.
	//
	for (int i = 0; i < _dim.row; i++) {

		area.blocks[1][i] = RULES_MARKER;

		area.blocks[i][2] = RULES_MARKER;
	}

	//
	//  Create the rules and remove the squares.
	//
	rules_create_game(&area);

	const int count = rules_remove_squares_lines(&area);

	//
	// Ensure that the result is as expected.
	//
	ut_check_int(count, 11, "removed");

	check_empty(area.blocks, _dim.row, _dim.col);

	//
	// Free the allocated area.
	//
	rules_free_game(&area);

	s_area_free(&area);
}

/******************************************************************************
 * The function checks the marking and removing of neighbors with the same
 * color.
 *
 * YY----
 * -Y----
 * -YXxxx
 * --x---
 * --xxxx
 * --x---
 *****************************************************************************/

static void test_check_neighbors() {

	//
	// Create and initialize the area
	//
	s_area area;
	s_area_create(&area, &_dim, &_size);
	s_area_set_blocks(&area, 0);

	area.blocks[0][0] = CLR_RED__N;
	area.blocks[0][1] = CLR_RED__N;

	area.blocks[1][1] = CLR_RED__N;

	area.blocks[2][1] = CLR_RED__N;
	area.blocks[2][2] = CLR_BLUE_N;
	area.blocks[2][3] = CLR_BLUE_N;
	area.blocks[2][4] = CLR_BLUE_N;
	area.blocks[2][5] = CLR_BLUE_N;

	area.blocks[3][2] = CLR_BLUE_N;

	area.blocks[4][2] = CLR_BLUE_N;
	area.blocks[4][3] = CLR_BLUE_N;
	area.blocks[4][4] = CLR_BLUE_N;
	area.blocks[4][5] = CLR_BLUE_N;

	area.blocks[5][2] = CLR_BLUE_N;

	//
	// Create the rules and remove the neighbors.
	//
	rules_create_game(&area);

	const int count = rules_remove_neighbors(&area);

	//
	// Ensure that the result is as expected.
	//
	ut_check_int(count, 14, "removed");

	check_empty(area.blocks, _dim.row, _dim.col);

	//
	// Free the allocated areas.
	//
	rules_free_game(&area);

	s_area_free(&area);
}

/******************************************************************************
 * The function is the a wrapper, that triggers the internal unit tests.
 *****************************************************************************/

void ut_rules_exec() {

	test_check_squares();

	test_check_lines();

	test_check_neighbors();
}
