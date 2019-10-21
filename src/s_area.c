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

void s_area_init(s_area *area) {

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
	if (area->size_row == 0 || area->size_col == 0) {
		log_exit_str("Size not set!");
	}
#endif

	block->row = (pixel->row - area->abs_row) / area->size_row;
	block->col = (pixel->col - area->abs_col) / area->size_col;

	log_debug("pixel - row: %d col: %d block - row: %d col: %d", pixel->row, pixel->col, block->row, block->col);
}

/******************************************************************************
 * The function checks whether a given pixel is inside one of the blocks of the
 * area or not.
 *****************************************************************************/

bool s_area_contains(const s_area *area, const s_point *pixel) {

	if (pixel->row < area->abs_row || pixel->col < area->abs_col) {
		return false;
	}

	if (pixel->row >= area->abs_row + area->blk_rows * area->size_row || pixel->col >= area->abs_col + area->blk_cols * area->size_col) {
		return false;
	}

	return true;
}
