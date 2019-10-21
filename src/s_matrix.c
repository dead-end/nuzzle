#include "common.h"
#include "s_matrix.h"

/******************************************************************************
 *
 *****************************************************************************/

s_area* s_area_create(const int rows, const int cols) {

	s_area *area = xmalloc(sizeof(s_area));

	area->blk_rows = rows;
	area->blk_cols = cols;

	area->block = xmalloc(sizeof(char*) * rows);

	for (int r = 0; r < rows; r++) {
		area->block[r] = xmalloc(sizeof(char) * cols);
	}

	return area;
}

/******************************************************************************
 *
 *****************************************************************************/

void s_area_init(s_area *area) {

	for (int row = 0; row < area->blk_rows; row++) {
		for (int col = 0; col < area->blk_cols; col++) {
			area->block[row][col] = 0;
		}
	}
}

/******************************************************************************
 *
 *****************************************************************************/

void s_area_free(s_area *area) {

	for (int r = 0; r < area->blk_rows; r++) {
		free(area->block[r]);
	}

	free(area->block);

	free(area);
}

/******************************************************************************
 *
 *****************************************************************************/

void s_area_get_index(const s_area *area, const s_point *pos, s_point *index) {

#ifdef DEBUG
	if (area->size_row == 0 || area->size_col == 0) {
		log_exit_str("Size not set!");
	}
#endif

	index->row = (pos->row - area->abs_row) / area->size_row;
	index->col = (pos->col - area->abs_col) / area->size_col;

	log_debug("pos - row: %d col: %d idx - row: %d col: %d", pos->row, pos->col, index->row, index->col);
}

/******************************************************************************
 *
 *****************************************************************************/

bool s_area_contains(const s_area *area, const s_point *point) {

	if (point->row < area->abs_row || point->col < area->abs_col) {
		return false;
	}

	if (point->row >= area->abs_row + area->blk_rows * area->size_row || point->col >= area->abs_col + area->blk_cols * area->size_col) {
		return false;
	}

	return true;
}
