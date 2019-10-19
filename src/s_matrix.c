#include "common.h"
#include "s_matrix.h"

/******************************************************************************
 *
 *****************************************************************************/

s_matrix* s_matrix_create(const int rows, const int cols) {

	s_matrix *matrix = xmalloc(sizeof(s_matrix));

	matrix->rows = rows;
	matrix->cols = cols;

	matrix->data = xmalloc(sizeof(char*) * rows);

	for (int r = 0; r < rows; r++) {
		matrix->data[r] = xmalloc(sizeof(char) * cols);
	}

	return matrix;
}

/******************************************************************************
 *
 *****************************************************************************/

void s_matrix_init(s_matrix *matrix) {
	for (int row = 0; row < matrix->rows; row++) {
		for (int col = 0; col < matrix->cols; col++) {
			matrix->data[row][col] = 0;
		}
	}
}

/******************************************************************************
 *
 *****************************************************************************/

void s_matrix_free(s_matrix *matrix) {

	for (int r = 0; r < matrix->rows; r++) {
		free(matrix->data[r]);
	}

	free(matrix->data);

	free(matrix);
}

/******************************************************************************
 *
 *****************************************************************************/

void s_matrix_get_index(const s_matrix *matrix, const s_point *pos, s_point *index) {

#ifdef DEBUG
	if (matrix->size_row == 0 || matrix->size_col == 0) {
		log_exit_str("Size not set!");
	}
#endif

	index->row = (pos->row - matrix->abs_row) / matrix->size_row;
	index->col = (pos->col - matrix->abs_col) / matrix->size_col;

	log_debug("pos - row: %d col: %d idx - row: %d col: %d", pos->row, pos->col, index->row, index->col);
}

/******************************************************************************
 *
 *****************************************************************************/

bool s_matrix_contains(const s_matrix *matrix, const s_point *point) {

	if (point->row < matrix->abs_row || point->col < matrix->abs_col) {
		return false;
	}

	if (point->row >= matrix->abs_row + matrix->rows * matrix->size_row || point->col >= matrix->abs_col + matrix->cols * matrix->size_col) {
		return false;
	}

	return true;
}
