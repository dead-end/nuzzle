#ifndef INC_S_AREA_H_
#define INC_S_AREA_H_

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

	s_point abs;

	s_point size;

} s_area;

#define s_point_set(p,r,c) (p)->row = (r);(p)->col = (c)

s_area* s_area_create(const int rows, const int cols);

#define s_area_set_abs(a,r,c) (a)->abs.row = (r); (a)->abs.col = (c)

#define s_area_set_size(a,r,c) (a)->size.row = (r); (a)->size.col = (c)

void s_area_free(s_area *area);

void s_area_abs_block(const s_area *area, const s_point *block, s_point *abs_blk);

#endif /* INC_S_AREA_H_ */
