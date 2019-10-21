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

typedef struct s_area {

	char **blocks;

	int blk_rows;

	int blk_cols;

	int abs_row;

	int abs_col;

	int size_row;

	int size_col;

} s_area;

#define s_point_set(p,r,c) (p)->row = (r);(p)->col = (c)

s_area* s_area_create(const int rows, const int cols);

#define s_area_set_abs(a,r,c) (a)->abs_row = (r); (a)->abs_col = (c)

#define s_area_set_size(a,r,c) (a)->size_row = (r); (a)->size_col = (c)

void s_area_init(s_area *area);

void s_area_free(s_area *area);

void s_area_get_block(const s_area *area, const s_point *pos, s_point *block);

bool s_area_contains(const s_area *area, const s_point *point);

#endif /* INC_S_MATRIX_H_ */
