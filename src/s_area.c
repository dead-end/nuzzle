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

#include "s_area.h"
#include "colors.h"

/******************************************************************************
 * The function copies one area to an other. The blocks are shared.
 *
 * (Unit tested)
 *****************************************************************************/

void s_area_copy(const s_area *from, s_area *to) {

	to->blocks = from->blocks;

	s_point_copy(&to->dim, &from->dim);
	s_point_copy(&to->pos, &from->pos);
	s_point_copy(&to->size, &from->size);
}

/******************************************************************************
 * The function creates a deep copy of the given area.
 *
 * (Unit tested)
 *****************************************************************************/

void s_area_copy_deep(const s_area *from, s_area *to) {

	s_point_copy(&to->dim, &from->dim);
	s_point_copy(&to->pos, &from->pos);
	s_point_copy(&to->size, &from->size);

	for (int i = 0; i < from->dim.row; i++) {
		for (int j = 0; j < from->dim.col; j++) {
			to->blocks[i][j] = from->blocks[i][j];
		}
	}
}

/******************************************************************************
 * The function returns a struct with the total size of the area.
 *
 * (Unit tested)
 *****************************************************************************/

s_point s_area_get_size(const s_area *area) {
	s_point result;

	result.row = area->dim.row * area->size.row;
	result.col = area->dim.col * area->size.col;

	log_debug("Size: %d/%d", result.row, result.col);

	return result;
}

/******************************************************************************
 * The function returns a structure with the lower right corner of the area.
 *
 * (Unit tested)
 *****************************************************************************/

s_point s_area_get_lr(const s_area *area) {
	s_point result;

	result.row = area->pos.row + area->dim.row * area->size.row - 1;
	result.col = area->pos.col + area->dim.col * area->size.col - 1;

	log_debug("Lower right: %d/%d", result.row, result.col);

	return result;
}

/******************************************************************************
 * The function returns the absolute position of the upper left corner of a
 * block of the given area.
 *
 * (Unit tested)
 *****************************************************************************/

s_point s_area_get_ul(const s_area *area, const s_point *idx) {
	s_point result;

	result.row = area->pos.row + area->size.row * idx->row;
	result.col = area->pos.col + area->size.col * idx->col;

	log_debug("Upper left: %d/%d", result.row, result.col);

	return result;
}

/******************************************************************************
 * The function check whether a given pixel (terminal character) is inside an
 * area, which is a two-dimensional array of blocks. The position of the area
 * is the upper left corner. The size is the number of pixels (characters) of
 * a block.
 *
 * (Unit tested)
 *****************************************************************************/

bool s_area_is_inside(const s_area *area, const int row, const int col) {

	bool result = true;

	//
	// Upper left corner
	//
	if (row < area->pos.row || col < area->pos.col) {
		result = false;
	}

	const s_point lower_right = s_area_get_lr(area);

	//
	// lower right corner
	//
	if (row > lower_right.row || col > lower_right.col) {
		result = false;
	}

	log_debug("pixel: %d/%d lb: %d/%d ub: %d/%d result: %s", row, col, area->pos.row, area->pos.col, lower_right.row, lower_right.col, bool_str(result));

	return result;
}

/******************************************************************************
 * The function checks whether the drop area is inside the given area.
 *
 * (Unit tested)
 *****************************************************************************/

bool s_area_is_area_inside(const s_area *outer_area, const s_area *inner_area) {

	//
	// Compare the positions of the two areas.
	//
	if (inner_area->pos.row < outer_area->pos.row || inner_area->pos.col < outer_area->pos.col) {
		return false;
	}

	//
	// Compare the lower right of the two areas.
	//
	const s_point outer_lr = s_area_get_lr(outer_area);
	const s_point inner_lr = s_area_get_lr(inner_area);

	if (inner_lr.row > outer_lr.row || inner_lr.col > outer_lr.col) {
		return false;
	}

	return true;
}

/******************************************************************************
 * The function checks if the current pixel is aligned with the s_area. This
 * does not mean that the pixel is inside the s_area.
 *
 * (Unit tested)
 *****************************************************************************/

bool s_area_is_aligned(const s_area *area, const int pos_row, const int pos_col) {
	return (pos_row - area->pos.row) % area->size.row == 0 && (pos_col - area->pos.col) % area->size.col == 0;
}

/******************************************************************************
 * The function aligns a point to the blocks of an area. It is assumed that the
 * point is inside the area, so the point is inside one of the blocks of the
 * area. The point is shifted to the upper left corner of the block where it is
 * inside. Before:
 *
 * aaaaAAAAaaaaAAAA (area)
 *       P          (point)
 *
 * After:
 *
 * aaaaAAAAaaaaAAAA (area)
 *     p            (point)
 *
 * (Unit tested)
 *****************************************************************************/

bool s_area_align_point(const s_area *area, s_point *point) {
	int diff;
	bool changed = false;

#ifdef DEBUG

	//
	// Check the precondition in debug mode
	//
	if (!s_area_is_inside(area, point->row, point->col)) {
		log_exit("Point: %d/%d is outside the area", point->row, point->col);
	}
#endif

	//
	// Align the row
	//
	diff = (point->row - area->pos.row) % area->size.row;

	if (diff != 0) {
		point->row -= diff;
		changed = true;
	}

	//
	// Align the col
	//
	diff = (point->col - area->pos.col) % area->size.col;

	if (diff != 0) {
		point->col -= diff;
		changed = true;
	}

	log_debug("pos: %d/%d aligned: %d/%d, changed: %s", area->pos.row, area->pos.col, point->row, point->col, bool_str(changed));

	//
	// The function returns true if the point changes, so a drawing is
	// necessary.
	//
	return changed;
}

/******************************************************************************
 * The function is called with an inner and an out area. We want to compute the
 * max position of the inner area, where the inner area is completely inside
 * the outer area.
 *
 * (Unit tested)
 *****************************************************************************/

s_point s_area_get_max_inner_pos(const s_area *outer_area, const s_area *inner_area) {

	//
	// Get the lower right point of the outer area
	//
	const s_point outer_lr = s_area_get_lr(outer_area);

	//
	// Get the size of the inner area
	//
	const s_point inner_size = s_area_get_size(inner_area);

	//
	// The max position it the difference of both values.
	//
	const s_point result = { outer_lr.row - inner_size.row + 1, outer_lr.col - inner_size.col + 1 };

	return result;
}

/******************************************************************************
 * The function is called with an outer area and an inner area, inside the
 * outer area. The inner area should be move inside the outer area.
 *
 * The function does not use the position of the inner area. It is expected
 * that the point is the current position of the inner area.
 *
 * (Unit tested)
 *****************************************************************************/

bool s_area_move_inner_area(const s_area *outer_area, const s_area *inner_area, s_point *point, const s_point *diff) {
	bool result = false;

#ifdef DEBUG

	//
	// Moving the inner area inside the outer area, the sizes have to be the same.
	//
	if (!s_point_same(&outer_area->size, &inner_area->size)) {
		log_exit_str("Sizes differ!");
	}

	//
	// The inner area has to be inside the outer area.
	//
	if (!s_area_is_area_inside(outer_area, inner_area)) {
		log_exit_str("Inner area is not inside the outer area!");
	}

	//
	// The inner and outer areas have to be aligned.
	//
	if (!s_area_is_aligned(outer_area, point->row, point->col)) {
		log_exit_str("Inner area is not aligned with the outer area!");
	}
#endif

	//
	// Get the maximal position of the inner area inside the outer area.
	//
	const s_point max_pos = s_area_get_max_inner_pos(outer_area, inner_area);

	//
	// Process the row
	//
	if (diff->row != 0) {
		const int row = point->row + diff->row * outer_area->size.row;
		if (row >= outer_area->pos.row && row <= max_pos.row) {
			point->row = row;
			result = true;
		}
	}

	//
	// Process the column
	//
	if (diff->col != 0) {
		const int col = point->col + diff->col * outer_area->size.col;
		if (col >= outer_area->pos.col && col <= max_pos.col) {
			point->col = col;
			result = true;
		}
	}

	return result;
}

/******************************************************************************
 * The function is called with an outer and an inner area. It moves the inner
 * area into the outer area. The new position is stored in the pos structure.
 * It is assumed that the inner area is outside the outer area.
 *
 * (Unit tested)
 *
 * TODO: currently not used
 *****************************************************************************/

void s_area_move_inside(const s_area *outer_area, const s_area *inner_area, s_point *pos) {

#ifdef DEBUG

	//
	// Check the precondition in debug mode.
	//
	if (s_area_is_area_inside(outer_area, inner_area)) {
		log_exit_str("Inner area is already inside the outer area");
	}
#endif

	const s_point max_pos = s_area_get_max_inner_pos(outer_area, inner_area);

	log_debug("pos: %d/%d", pos->row, pos->col);

	//
	// Adjust the row
	//
	if (inner_area->pos.row < outer_area->pos.row) {
		pos->row = outer_area->pos.row;

	} else if (inner_area->pos.row >= max_pos.row) {
		pos->row = max_pos.row;
	}

	//
	// Adjust the column
	//
	if (inner_area->pos.col < outer_area->pos.col) {
		pos->col = outer_area->pos.col;

	} else if (inner_area->pos.col >= max_pos.col) {
		pos->col = max_pos.col;
	}

	log_debug("pos: %d/%d", pos->row, pos->col);
}

/******************************************************************************
 * The function gets the block for an absolute pixel.
 *****************************************************************************/

void s_area_get_block(const s_area *area, const s_point *pixel, s_point *block) {

	//
	// Ensure that the pixel is inside the area
	//
	if (!s_area_is_inside(area, pixel->row, pixel->col)) {
		log_exit("pixel: %d/%d not inside", pixel->row, pixel->col);
	}

	block->row = (pixel->row - area->pos.row) / area->size.row;
	block->col = (pixel->col - area->pos.col) / area->size.col;

	log_debug("pixel - row: %d col: %d block - row: %d col: %d", pixel->row, pixel->col, block->row, block->col);
}

/******************************************************************************
 * The function sets the blocks of the area to a given value. It is used to
 * reset the s_area.
 *****************************************************************************/

void s_area_set_blocks(const s_area *area, const t_block value) {

	for (int row = 0; row < area->dim.row; row++) {
		for (int col = 0; col < area->dim.col; col++) {
			area->blocks[row][col] = value;
		}
	}
}

/******************************************************************************
 * The function checks if the position of the area is the given row and column.
 *****************************************************************************/

bool s_area_same_pos(const s_area *area, const int pos_row, const int pos_col) {

	const bool result = pos_row == area->pos.row && pos_col == area->pos.col;
	log_debug("Position %d/%d  is the same: %d", pos_row, pos_col, result);

	return result;
}

/******************************************************************************
 * The functions creates a s_area structure, by setting the values and
 * allocating the blocks.
 *****************************************************************************/

void s_area_create(s_area *area, const s_point *dim, const s_point *size) {
	log_debug("Creating area with dim: %d/%d size: %d/%d", dim->row, dim->col, size->row, size->col);

	s_point_set(&area->dim, dim->row, dim->col);

	s_point_set(&area->size, size->row, size->col);

	area->blocks = blocks_create(area->dim.row, area->dim.col);
}

/******************************************************************************
 * The function frees the s_area struct. This is done by freeing its blocks.
 *****************************************************************************/

void s_area_free(s_area *area) {
	log_debug_str("Freeing area.");

	blocks_free(area->blocks, area->dim.row);
}

/******************************************************************************
 * The function computes the effective lower right corner of the used area. An
 * area may contain rows / cols of blocks that are completely empty.
 *****************************************************************************/

static void s_area_get_eff_lr(const s_area *area, s_point *lr) {

	//
	// Initialize the lower right corner with a minimal, none valid value.
	//
	s_point_set(lr, -1, -1);

	for (int row = 0; row < area->dim.row; row++) {
		for (int col = 0; col < area->dim.col; col++) {

			if (area->blocks[row][col] == CLR_NONE) {
				continue;
			}

			//
			// Increase the corner if necessary.
			//
			if (lr->row < row) {
				lr->row = row;
			}

			if (lr->col < col) {
				lr->col = col;
			}
		}
	}

	log_debug("lower right: %d/%d", lr->row, lr->col);
}

/******************************************************************************
 * The function computes the effective upper left corner of the used area. An
 * area may contain rows / cols of blocks that are completely empty.
 *****************************************************************************/

void s_area_get_eff_ul(const s_area *area, s_point *ul) {

	//
	// Initialize the upper left corner with a maximal, none valid value.
	//
	ul->row = area->dim.row;
	ul->col = area->dim.col;

	//
	// Compute the upper left corner of the used area.
	//
	for (int row = 0; row < area->dim.row; row++) {
		for (int col = 0; col < area->dim.col; col++) {

			if (area->blocks[row][col] == CLR_NONE) {
				continue;
			}

			//
			// Decrease the upper left corner
			//
			if (ul->row > row) {
				ul->row = row;
			}

			if (ul->col > col) {
				ul->col = col;
			}
		}
	}

	log_debug("upper left: %d/%d", ul->row, ul->col);
}

/******************************************************************************
 * The function normalizes a newly created drop area. This is done by removing
 * empty rows and columns at the beginning. Example:
 *
 * 0 0 0
 * 0 1 1
 * 0 1 0
 *
 * To:
 *
 * 1 1 0
 * 1 0 0
 * 0 0 0
 *
 * ATTENTION: The dimension is updated, in this case to (2,2)
 *****************************************************************************/

void s_area_normalize(s_area *area) {

	//
	// Get the upper left corner of the used area.
	//
	s_point ul;
	s_area_get_eff_ul(area, &ul);

	log_debug("ul: %d/%d -> 0/0", ul.row, ul.col);

	//
	// Step: 1
	//
	// At the first step, we want to move the used area to the upper left
	// corner.
	//
	if (ul.row != 0 || ul.col != 0) {

		//
		// Copy the used area to the origin: 0/0
		//
		for (int row = 0; row < area->dim.row; row++) {
			for (int col = 0; col < area->dim.col; col++) {

				//
				// Ensure that the target is part of the area
				//
				if (row + ul.row < area->dim.row && col + ul.col < area->dim.col) {
					area->blocks[row][col] = area->blocks[row + ul.row][col + ul.col];

				}
				//
				// If the index is outside fill the blocks with NONE.
				//
				else {
					area->blocks[row][col] = CLR_NONE;
				}
			}
		}
	}

	//
	// Step: 2
	//
	// At this step we want to remove the unused rows and columns at the end of
	// the used area. This is done by updating the dimension of the used area.
	// The requires that the dimension is set every time a block to drop is
	// produced.
	//
	s_point lr;
	s_area_get_eff_lr(area, &lr);
	s_point_set(&area->dim, lr.row + 1, lr.col + 1);
	log_debug("new dim: %d/%d", area->dim.row, area->dim.col);
}

/******************************************************************************
 * The function check whether the drop area can be dropped anywhere on the
 * other area. If the index structure is not null, then the index of the first
 * position is stored in the structure.
 *****************************************************************************/

bool s_area_can_drop_anywhere(s_area *area, s_area *drop_area, s_point *idx) {

	//
	// Compute the end index to ensure that the drop area fits in the other.
	//
	const int row_end = area->dim.row - drop_area->dim.row;
	const int col_end = area->dim.col - drop_area->dim.col;

	log_debug("Check from 0/0 to %d,%d", row_end, col_end);

	s_point start;

	for (start.row = 0; start.row <= row_end; start.row++) {
		for (start.col = 0; start.col <= col_end; start.col++) {

			//
			// Check if the drop area can be dropped at this place.
			//
			if (s_area_drop(area, &start, drop_area, false)) {
				log_debug("Can drop at index: %d/%d", start.row, start.col);

				//
				// Store the index, if requested.
				//
				if (idx != NULL) {
					s_point_copy(idx, &start);
				}
				return true;
			}
		}
	}

	return false;
}

/******************************************************************************
 * The function can be used to drop an area on the game area or to check if it
 * is possible. It is assumed that the drop area fits in the game area. So the
 * function checks whether there is already a block at the position, which
 * blocks the dropping.
 *****************************************************************************/

bool s_area_drop(s_area *area, const s_point *idx, const s_area *drop_area, const bool do_drop) {

	for (int row = 0; row < drop_area->dim.row; row++) {
		for (int col = 0; col < drop_area->dim.col; col++) {

			//
			// If the block of the used area is not defined, there is nothing
			// to do or to check.
			//
			if (drop_area->blocks[row][col] == CLR_NONE) {
				continue;
			}

			//
			// If there is a block on the game area defined, dropping is not
			// possible.
			//
			if (area->blocks[idx->row + row][idx->col + col] != CLR_NONE) {
				return false;
			}

			//
			// At this point dropping of current block is possible. The flag
			// defines whether the dropping should be performed.
			//
			if (do_drop) {
				area->blocks[idx->row + row][idx->col + col] = drop_area->blocks[row][col];
			}
		}
	}

	log_debug("can drop at: %d/%d", idx->row, idx->col);

	return true;
}

/******************************************************************************
 * The function print a block of a s_area, with a given character. It is
 * assumed that the color is already set.
 *****************************************************************************/

void s_area_print_block(WINDOW *win, const s_area *area, const s_point *idx, const wchar_t ch) {

	//
	// Get the upper left corner of the block with the given index.
	//
	const s_point ul = s_area_get_ul(area, idx);

	//
	// Get the lower right corner of the block with the given index.
	//
	const s_point lr = { ul.row + area->size.row, ul.col + area->size.col };

	for (int row = ul.row; row < lr.row; row++) {
		for (int col = ul.col; col < lr.col; col++) {
			mvwprintw(win, row, col, "%lc", ch);
		}
	}
}

/******************************************************************************
 * The function computes the color of the game area depending on the chess
 * pattern type.
 *****************************************************************************/

// TODO: check if this should be part of the colors_chess_attr_char() function
static t_block get_ga_color(const s_area *area, const s_point *idx, const e_chess_type chess_type) {

	//
	// We need the primary color of the game area.
	//
	const t_block ga_color = area->blocks[idx->row][idx->col];

	//
	// We check if a modification is necessary.
	//
	if (chess_type == CHESS_DOUBLE && ga_color != CLR_NONE && colors_is_even((idx->row / 3), (idx->col / 3))) {
		return CLR_MK_L;
	}

	return ga_color;
}

/******************************************************************************
 * The function prints an empty area with a chess pattern. This can be used as
 * an initialization.
 *****************************************************************************/

void s_area_print_chess(WINDOW *win, const s_area *area, const e_chess_type chess_type) {
	wchar_t chr;
	t_block ga_color;
	s_point idx;

	//
	// Iterate through the blocks of the game area.
	//
	for (idx.row = 0; idx.row < area->dim.row; idx.row++) {
		for (idx.col = 0; idx.col < area->dim.col; idx.col++) {

			//
			// Get the color of the game area depending on the chess pattern
			// type.
			//
			ga_color = get_ga_color(area, &idx, chess_type);

			//
			// Set the color pair and get the character to display.
			//
			chr = colors_chess_attr_char(win, ga_color, CLR_NONE, &idx, chess_type);

			//
			// Print the block with a given color and character.
			//
			s_area_print_block(win, area, &idx, chr);
		}
	}
}

/******************************************************************************
 * The function prints a pixel with a given color. The background is a chess
 * pattern.
 *****************************************************************************/

void s_area_print_chess_pixel(WINDOW *win, const s_area *area, const s_point *pixel, const t_block da_color, const e_chess_type chess_type) {

	log_debug("pixel: %d/%d, fg-color: %d", pixel->row, pixel->col, da_color);

	//
	// Get the background color of the pixel. We need the corresponding game
	// block, which contains the color.
	//
	s_point block_idx;
	s_area_get_block(area, pixel, &block_idx);

	//
	// Get the color of the game area depending on the chess pattern type.
	//
	const t_block ga_color = get_ga_color(area, &block_idx, chess_type);

	//
	// Set the color pair and get the character to display.
	//
	const wchar_t chr = colors_chess_attr_char(win, ga_color, da_color, &block_idx, chess_type);

	//
	// Print the character at the position.
	//
	mvwprintw(win, pixel->row, pixel->col, "%lc", chr);
}
