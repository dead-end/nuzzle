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

int rules_remove_squares_lines(s_area *area, t_block **marks) {

	rule_reset_marks(area, marks);

	rules_mark_squares(area, marks);

	rules_mark_lines(area, marks);

	return rules_remove_marked(area, marks);
}

/******************************************************************************
 * The function is called with a color and recursively marks all neighbors,
 * that have the same color. The recursion stops if all neighbors have
 * different colors or are already marked.
 *****************************************************************************/

static void rules_mark_neighbors(const s_area *area, t_block **marks, const int row, const int col, t_block color, int *num) {

	//
	// Ensure that we are on the game area. The function is called on the
	// neighbors of a block by adding and subtracting 1 to the row / column.
	//
	if (row < 0 || row >= area->dim.row || col < 0 || col >= area->dim.row) {
		log_debug("Outside: %d/%d num: %d color: %d", row, col, *num, color);
		return;
	}

	//
	// Current block has the wrong color.
	//
	if (area->blocks[row][col] != color) {
		log_debug("Wrong color: %d/%d num: %d color: %d", row, col, *num, color);
		return;
	}

	//
	// Current block is already marked.
	//
	if (marks[row][col] != 0) {
		log_debug("Already marked: %d/%d num: %d color: %d", row, col, *num, color);
		return;
	}

	//
	// Increase the number and mark the block.
	//
	marks[row][col] = ++(*num);

	log_debug("Mark: %d/%d num: %d color: %d", row, col, *num, color);

	//
	// Recursively process the neighbors.
	//
	rules_mark_neighbors(area, marks, row + 1, col, color, num);
	rules_mark_neighbors(area, marks, row - 1, col, color, num);
	rules_mark_neighbors(area, marks, row, col + 1, color, num);
	rules_mark_neighbors(area, marks, row, col - 1, color, num);
}

/******************************************************************************
 * The function is called after an area is dropped. It checks if blocks of the
 * game area should disappear. If more than 4 blocks with the same color are
 * neighbors, they should be removed.
 *
 * The function returns the number of removed blocks.
 *****************************************************************************/

int rules_remove_neighbors(s_area *area, const s_point *idx, const s_point *dim, t_block **marks) {
	int total = 0;
	int num;

	t_block color;

	//
	// Iterate over the blocks of the drop area.
	//
	for (int row = 0; row < dim->row; row++) {
		for (int col = 0; col < dim->col; col++) {

			color = area->blocks[row][col];

			//
			// If the current, dropped block of the area has no color, it
			// cannot be the trigger for a removing.
			//
			if (color == CLR_NONE) {
				log_debug("drop (used) empty: %d/%d", row, col);
				continue;
			}

			//
			// If the block of the dropped area has a color, we mark all
			// neighbors with the same color.
			//
			num = 0;
			rule_reset_marks(area, marks);
			rules_mark_neighbors(area, marks, idx->row + row, idx->col + col, color, &num);
			log_debug("num: %d", num);

			//
			// If the number of neighbors with the same color is more than
			// 4, we have to remove the marks from the game area.
			//
			if (num >= 4) {
				rules_remove_marked(area, marks);
				total += num;
			}
		}
	}

	return total;
}
