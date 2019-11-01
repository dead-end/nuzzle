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
 * The function computes the absolute position of the block, which is the upper
 * left pixel.
 *****************************************************************************/

void s_area_abs_block(const s_area *area, const s_point *block, s_point *abs_blk) {

	abs_blk->row = area->abs.row + area->size.row * block->row;
	abs_blk->col = area->abs.col + area->size.col * block->col;
}
