#include <s_area.h>
#include "common.h"
#include "colors.h"

/******************************************************************************
 * The function creates a s_area which consists of an 2 dimensional array of
 * blocks.
 *****************************************************************************/

s_area* s_area_create(const int blk_rows, const int blk_cols) {

	s_area *area = xmalloc(sizeof(s_area));

	area->blk_rows = blk_rows;
	area->blk_cols = blk_cols;

	area->blocks = xmalloc(sizeof(char*) * blk_rows);

	for (int r = 0; r < blk_rows; r++) {
		area->blocks[r] = xmalloc(sizeof(char) * blk_cols);
	}

	return area;
}

/******************************************************************************
 * The function initializes the s_area with color_none.
 *****************************************************************************/

void s_area_init_null(s_area *area) {

	for (int row = 0; row < area->blk_rows; row++) {
		for (int col = 0; col < area->blk_cols; col++) {
			area->blocks[row][col] = color_none;
		}
	}
}

/******************************************************************************
 * The function frees the allocated data.
 *****************************************************************************/

void s_area_free(s_area *area) {

	for (int r = 0; r < area->blk_rows; r++) {
		free(area->blocks[r]);
	}

	free(area->blocks);

	free(area);
}

/******************************************************************************
 * The function gets the block for an absolute pixel.
 *****************************************************************************/

void s_area_get_block(const s_area *area, const s_point *pixel, s_point *block) {

#ifdef DEBUG
	if (area->size.row == 0 || area->size.col == 0) {
		log_exit_str("Size not set!");
	}
#endif

	block->row = (pixel->row - area->abs.row) / area->size.row;
	block->col = (pixel->col - area->abs.col) / area->size.col;

	log_debug("pixel - row: %d col: %d block - row: %d col: %d", pixel->row, pixel->col, block->row, block->col);
}

/******************************************************************************
 * The function checks whether a given pixel is inside one of the blocks of the
 * area or not.
 *****************************************************************************/

bool s_area_contains(const s_area *area, const s_point *pixel) {

	if (pixel->row < area->abs.row || pixel->col < area->abs.col) {
		return false;
	}

	if (pixel->row >= area->abs.row + area->blk_rows * area->size.row || pixel->col >= area->abs.col + area->blk_cols * area->size.col) {
		return false;
	}

	return true;
}

/******************************************************************************
 * The function initializes a s_area with random blocks. It is ensured that at
 * least one block is not empty.
 *****************************************************************************/

void s_area_init_random(s_area *area) {
	int count = 0;

	for (int row = 0; row < area->blk_rows; row++) {
		for (int col = 0; col < area->blk_cols; col++) {

			//
			// First check if the color is set
			//
			if (rand() % 100 < 60) {
				area->blocks[row][col] = color_none;

			} else {
				area->blocks[row][col] = (rand() % 4) + 1;
				count++;
			}

			log_debug("row: %d col: %d color: %d", row, col, area->blocks[row][col]);
		}
	}

	//
	// If all blocks are empty, try again.
	//
	if (count == 0) {
		log_debug_str("Filling failed, try again!");

		s_area_init_random(area);
	}
}

/******************************************************************************
 * The function computes the absolute position of the block, which is the upper
 * left pixel.
 *****************************************************************************/

void s_area_abs_block(const s_area *area, const s_point *block, s_point *abs_blk) {

	abs_blk->row = area->abs.row + area->size.row * block->row;
	abs_blk->col = area->abs.col + area->size.col * block->col;
}
