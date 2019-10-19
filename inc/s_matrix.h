#ifndef INC_S_MATRIX_H_
#define INC_S_MATRIX_H_

#include <stdbool.h>

/******************************************************************************
 *
 *****************************************************************************/

typedef struct s_point {

	int row;

	int col;

} s_point;

/******************************************************************************
 *
 *****************************************************************************/

typedef struct s_matrix {

	char **data;

	int rows;

	int cols;

	int abs_row;

	int abs_col;

	int size_row;

	int size_col;

} s_matrix;

#define s_point_set(p,r,c) (p)->row = (r);(p)->col = (c)

s_matrix* s_matrix_create(const int rows, const int cols);

#define s_matrix_set_abs(m,r,c) (m)->abs_row = (r); (m)->abs_col = (c)

#define s_matrix_set_size(m,r,c) (m)->size_row = (r); (m)->size_col = (c)

void s_matrix_init(s_matrix *matrix);

void s_matrix_free(s_matrix *matrix);

void s_matrix_get_index(const s_matrix *matrix, const s_point *pos, s_point *index);

bool s_matrix_contains(const s_matrix *matrix, const s_point *point);

#endif /* INC_S_MATRIX_H_ */
