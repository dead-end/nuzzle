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

#include "colors.h"
#include "rules.h"

/******************************************************************************
 * The function marks horizontal and vertical lines.
 *****************************************************************************/

static void rules_mark_lines(const s_area *area, t_block **marks) {

	bool is_complete;

	//
	// Process rows.
	//
	for (int row = 0; row < area->dim.row; row++) {

		is_complete = true;

		for (int col = 0; col < area->dim.col; col++) {

			if (area->blocks[row][col] == CLR_NONE) {
				is_complete = false;
				break;
			}
		}

		if (is_complete) {

			log_debug("Mark line at row: %d", row);

			for (int col = 0; col < area->dim.col; col++) {
				marks[row][col] = RULES_MARKER;
			}
		}
	}

	//
	// Process columns.
	//
	for (int col = 0; col < area->dim.col; col++) {

		is_complete = true;

		for (int row = 0; row < area->dim.row; row++) {

			if (area->blocks[row][col] == CLR_NONE) {
				is_complete = false;
				break;
			}
		}

		if (is_complete) {

			log_debug("Mark line at col: %d", col);

			for (int row = 0; row < area->dim.row; row++) {
				marks[row][col] = RULES_MARKER;
			}
		}
	}
}

/******************************************************************************
 * The function checks if all blocks of a square (3x3) are set.
 *****************************************************************************/

static void rules_mark_square(const s_area *area, t_block **marks, const int start_row, const int start_col) {

	//
	// Compute the end of the squares
	//
	const int end_row = start_row + RULES_SQUARE_DIM;
	const int end_col = start_col + RULES_SQUARE_DIM;

	//
	// Ensure that all blocks of the square are set.
	//
	for (int row = start_row; row < end_row; row++) {
		for (int col = start_col; col < end_col; col++) {

			//
			// If we found a block that is not set, we do not need to check
			// more.
			//
			if (area->blocks[row][col] == CLR_NONE) {
				return;
			}
		}
	}

	log_debug("Mark square at: %d/%d", start_row, start_col);

	//
	// If all the blocks are set in the area, we can mark the square in the
	// marks array.
	//
	for (int row = start_row; row < end_row; row++) {
		for (int col = start_col; col < end_col; col++) {
			marks[row][col] = RULES_MARKER;
		}
	}
}

/******************************************************************************
 * The function checks the squares (3x3) in the area and marks squares that are
 * completely set.
 *****************************************************************************/

static void rules_mark_squares(s_area *area, t_block **marks) {

	for (int row = 0; row < area->dim.row; row = row + RULES_SQUARE_DIM) {
		for (int col = 0; col < area->dim.col; col = col + RULES_SQUARE_DIM) {

			rules_mark_square(area, marks, row, col);
		}
	}
}

/******************************************************************************
 * The function removes blocks from the area, which are marked in the marks
 * array and returns the number of blocks that were removed.
 *****************************************************************************/

static int rules_remove_marked(s_area *area, t_block **marks) {
	int count = 0;

	for (int row = 0; row < area->dim.row; row++) {
		for (int col = 0; col < area->dim.col; col++) {

			//
			// If the block is marked, we can remove it from the game.
			//
			if (marks[row][col] != CLR_NONE) {
				area->blocks[row][col] = CLR_NONE;
				count++;
			}
		}
	}

	log_debug("Removed: %d blocks.", count);

	return count;
}

/******************************************************************************
 * The function resets the array with the marks.
 *****************************************************************************/

static void rule_reset_marks(const s_area *area, t_block **marks) {

	for (int row = 0; row < area->dim.row; row++) {
		for (int col = 0; col < area->dim.col; col++) {

			if (marks[row][col] != CLR_NONE) {
				marks[row][col] = CLR_NONE;
			}
		}
	}
}

/******************************************************************************
 * The function applies rules to the area. 3x3 squares and horizontal and
 * vertical lines are removed. The function returns the number of blacks that
 * are removed.
 *****************************************************************************/

int rules_mark_squares_lines(s_area *area, t_block **marks) {

	rule_reset_marks(area, marks);

	rules_mark_squares(area, marks);

	rules_mark_lines(area, marks);

	return rules_remove_marked(area, marks);
}
