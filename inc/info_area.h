#ifndef INC_INFO_AREA_H_
#define INC_INFO_AREA_H_

void info_area_init(const int hs);

void info_area_add_to_score(const int add_2_score);

void info_area_set_pos(const int row, const int col);

void info_area_print();

bool info_area_contains(const s_point *pixel);

void info_area_print_pixel(const s_point *pixel, enum e_colors color);

#endif /* INC_INFO_AREA_H_ */
