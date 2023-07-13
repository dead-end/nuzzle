/*
 * MIT License
 *
 * Copyright (c) 2020 dead-end
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

#include "home_area.h"
#include "colors.h"

 /******************************************************************************
  * The home area consists of areas that can be picked up. They are stored in an
  * array with at most HOME_MAX entries. The actual number of entries is
  * _home_num.
  *****************************************************************************/

typedef struct s_home {

	//
	// The area which can be picked up.
	//
	s_area area;

	//
	// A flag that says that the home area was successfully dropped.
	//
	bool droped;

} s_home;

#define HOME_MAX 3

static int _home_num;

static s_home _home_area[HOME_MAX];

/******************************************************************************
 * The variable contains the index of the home area, which is currently picked
 * up or PICKUP_IDX_UNDEF if no home area is picked up.
 *
 * The parameter has to be consistent with the s_status struct, which also has
 * a parameter (see offset), which indicates that a drop area is picked up.
 *****************************************************************************/

static int _pickup_idx;

#define PICKUP_IDX_UNDEF -1

/******************************************************************************
 * We need blocks to store the currently picked up home area. The drop area is
 * normalized, which means, empty rows and columns are removed. So we cannot
 * use the drop area as a backup.
 *****************************************************************************/

static t_block **_blocks = NULL;

static s_point _blocks_dim;

/******************************************************************************
 * The function ensures that a home area is picked up.
 *****************************************************************************/

#ifdef DEBUG

static inline void ensure_picked_up() {

	//
	// Ensure that the index is set and in the valid range.
	//
	if (_pickup_idx < 0 || _pickup_idx >= HOME_MAX) {
		log_exit("Index out of range: %d", _pickup_idx);
	}

	//
	// Ensure that it is not dropped.
	//
	if (_home_area[_pickup_idx].droped) {
		log_exit("Already dropped: %d", _pickup_idx);
	}
}

#endif

/******************************************************************************
 * The function ensures that a home area is NOT picked up.
 *****************************************************************************/

#ifdef DEBUG

static inline void ensure_not_picked_up() {

	//
	// Ensure that nothing is picked up.
	//
	if (_pickup_idx != PICKUP_IDX_UNDEF) {
		log_exit("Home area is picked up: %d", _pickup_idx);
	}
}

#endif

/******************************************************************************
 * The function is called with the absolute position of a pixel and returns the
 * index of the home area that contains the pixel or -1 if none of the home
 * areas contains it.
 *
 * Each home area has a position so the function works independent of the
 * layout of the home areas.
 *****************************************************************************/

int home_area_get_idx(const s_point *pixel) {
	int idx = PICKUP_IDX_UNDEF;

	for (int i = 0; i < _home_num; i++) {

		//
		// Check if the current home area contains the pixel.
		//
		if (s_area_is_inside(&_home_area[i].area, pixel)) {
			idx = i;
			break;
		}
	}

	log_debug("Home area: %d contains: %d/%d", idx, pixel->row, pixel->col);

	return idx;
}

/******************************************************************************
 * The function checks if one of the home areas can be dropped on the game
 * area, to test if the game actually ended. Areas that are already dropped are
 * ignored.
 *
 * The function is called after an area is dropped. So there should not be a
 * home area picked up.
 *****************************************************************************/

bool home_area_can_drop_anywhere(s_area *area) {
	bool result = false;

	//
	// Create an area and use the backup blocks. To to this, we need to ensure
	// that nothing is picked up. In this case the blocks are not in used.
	//
	s_area norm_area = { .blocks = _blocks };

#ifdef DEBUG

	//
	// If a home area is picked up, the blocks of the s_area are empty.
	// So checking for dropping makes no sense.
	//
	ensure_not_picked_up();
#endif

	for (int i = 0; i < _home_num; i++) {

		//
		// If the home area is already dropped, there is nothing to check.
		// Being dropped means that the home area is empty and not in use.
		//
		if (_home_area[i].droped) {
			continue;
		}

		s_area_copy_deep(&_home_area[i].area, &norm_area);
		s_area_normalize(&norm_area);

		//
		// Check if the current home area can be dropped on the game area.
		//
		if (s_area_can_drop_anywhere(area, &norm_area, NULL)) {
			result = true;
			break;
		}
	}

	log_debug("One of the home areas can be dropped: %s", bool_str(result));

	return result;
}

/******************************************************************************
 * The function marks a home area as dropped. The home area is empty until the
 * areas are refilled.
 *****************************************************************************/

void home_area_mark_drop() {

#ifdef DEBUG

	//
	// Ensure preconditions.
	//
	ensure_picked_up();
#endif

	_home_area[_pickup_idx].droped = true;

	_pickup_idx = PICKUP_IDX_UNDEF;
}

/******************************************************************************
 * The function checks if all home areas are dropped. In this case refilling is
 * required.
 *****************************************************************************/

static bool home_area_needs_refilling() {
	bool result = true;

	for (int i = 0; i < _home_num; i++) {

		//
		// If one of the home areas is not dropped, we need no refilling
		//
		if (!_home_area[i].droped) {
			result = false;
			break;
		}
	}

	log_debug("Needs refilling: %s", bool_str(result));

	return result;
}

/******************************************************************************
 * If all home areas are empty, they have to be filled again. In this case the
 * function returns true, which indicates that the home areas have to printed
 * again.
 *
 * The force flag is used when we want to reset the game. In this case we do
 * not want to check if refilling is necessary.
 *****************************************************************************/

bool home_area_refill(const s_game_cfg *game_cfg, const bool force) {

#ifdef DEBUG

	//
	// Ensure preconditions.
	//
	ensure_not_picked_up();
#endif

	//
	// If not all home areas are empty, there is nothing to do.
	//
	if (!force && !home_area_needs_refilling()) {
		return false;
	}

	for (int i = 0; i < _home_num; i++) {
		log_debug("Filling home area: %d", i);

		//
		// Call the configured refilling function
		//
		(*game_cfg->fct_ptr_init_random)(game_cfg, _home_area[i].area.blocks);

		//
		// Remove the dropped mark, which is definitely set at this point.
		//
		_home_area[i].droped = false;
	}

	return true;
}

/******************************************************************************
 * The function picks up a home area if possible. If so, the data is copied to
 * the drop area and a backup is created.
 *
 * The function returns true if a home area was picked up and false if this is
 * not possible.
 *****************************************************************************/

bool home_area_pickup(s_area *area, const s_point *pixel) {
	log_debug("picking up home area at: %d/%d", pixel->row, pixel->col);

	_pickup_idx = home_area_get_idx(pixel);

	//
	// The function returns PICKUP_IDX_UNDEF if the event is outside the home
	// area. In this case, there is no index.
	//
	if (_pickup_idx == PICKUP_IDX_UNDEF) {
		log_debug_str("Event is outside the homearea!");
		return false;
	}

	//	
	// If the event is inside the home area, we have an index, but we have to 
	// ensure that the area is not already empty, which means dropped.
	//
	if (_home_area[_pickup_idx].droped) {
		log_debug("Home area with idx: %d is already dropped!", _pickup_idx);
		return false;
	}

#ifdef DEBUG

	//
	// Ensure that the picking up was successful.
	//
	ensure_picked_up();
#endif

	//
	// Save the picked up area, which is later used.
	//
	s_area *home_area = &_home_area[_pickup_idx].area;

	//
	// The position of the drop area is the effective upper left corner.
	//
	s_point ul_idx;
	s_area_get_eff_ul(home_area, &ul_idx);
	area->pos = s_area_get_ul(home_area, &ul_idx);

	//
	// Reset the dimension of the drop area, which is normalized.
	//
	s_point_copy(&area->dim, &home_area->dim);

	//
	// Copy the home area blocks to the drop area.
	//
	blocks_copy(home_area->blocks, area->blocks, &home_area->dim);

	//
	// Create a backup of the home area. The drop area is normalized, so we
	// cannot use it as a backup.
	//
	blocks_copy(home_area->blocks, _blocks, &home_area->dim);

	//
	// Delete the content of the picked up home area.
	//
	s_area_set_blocks(home_area, CLR_NONE);

	//
	// Normalize the drop area, by removing the empty rows / columns.
	//
	s_area_normalize(area);

	return true;
}

/******************************************************************************
 * The function moves a picked up home area to its position again.
 *****************************************************************************/

void home_area_undo_pickup() {

#ifdef DEBUG

	//
	// Ensure preconditions.
	//
	ensure_picked_up();
#endif

	//
	// Copy the backup to the home area.
	//
	blocks_copy(_blocks, _home_area[_pickup_idx].area.blocks, &_home_area[_pickup_idx].area.dim);

	_pickup_idx = PICKUP_IDX_UNDEF;

	log_debug("Home area restored: %d", _pickup_idx);
}

/******************************************************************************
 * The function returns the chess type for a home area. If the chess type of
 * the game is CHESS_DOUBLE, then the chess type of the home areas is toggling.
 *****************************************************************************/

static short home_area_get_chess_type(const s_status *status, const int home_area_idx) {

	//
	// On CHESS_DOUBLE even home areas have a CHESS_SIMPLE_DARK and odd have a
	// CHESS_SIMPLE_LIGHT chess pattern.
	//
	if (status->game_cfg->chess_type == CHESS_DOUBLE) {
		return home_area_idx % 2 == 0 ? CHESS_SIMPLE_DARK : CHESS_SIMPLE_LIGHT;
	}

	return status->game_cfg->chess_type;
}

/******************************************************************************
 * The function prints all home areas.
 *****************************************************************************/

void home_area_print(WINDOW *win, const s_status *status) {

	for (int i = 0; i < _home_num; i++) {

		log_debug("Processing home area: %d", i);

		s_area_print_chess(win, &_home_area[i].area, home_area_get_chess_type(status, i));
	}
}

/******************************************************************************
 * The function prints a pixel. It is assumed that the pixel is inside one of
 * the home areas.
 *****************************************************************************/

void home_area_print_pixel(WINDOW *win, const s_status *status, const s_point *pixel, const t_block da_color) {

	//
	// Get the index of the affected home area.
	//
	const int idx = home_area_get_idx(pixel);

	//
	// Ensure that is is value.
	//
	if (idx < 0) {
		log_exit("Index not found for pixel: %d/%d", pixel->row, pixel->col);
	}

	log_debug("pixel: %d/%d (idx: %d)", pixel->row, pixel->col, idx);

	//
	// Print the pixel with a chess pattern as a background.
	//
	s_area_print_chess_pixel(win, &_home_area[idx].area, pixel, da_color, home_area_get_chess_type(status, idx));
}

/******************************************************************************
 * The function resets the home areas. The picked up mark is removed and the
 * home areas are refilled, independent of their state.
 *****************************************************************************/

void home_area_reset(const s_game_cfg *game_cfg) {

	_pickup_idx = PICKUP_IDX_UNDEF;

	home_area_refill(game_cfg, true);
}

/******************************************************************************
 * The function frees the home area. It has to called each time a game has
 * ended.
 *****************************************************************************/

void home_area_free_game() {

	//
	// Check if there is something to free.
	//
	if (_blocks == NULL) {
		log_debug_str("Already freed!");
		return;
	}

	for (int i = 0; i < _home_num; i++) {
		s_area_free(&_home_area[i].area);
	}

	blocks_free(_blocks, _blocks_dim.row);

	//
	// Mark as freed
	//
	_blocks = NULL;
}

/******************************************************************************
 * The function creates the home areas. It has to be called each time a game
 * starts.
 *****************************************************************************/

void home_area_create_game(const s_game_cfg *game_cfg) {

	//
	// Ensure that the number of home areas is valid.
	//
	if (game_cfg->home_num > HOME_MAX) {
		log_exit("Number of home areas too large: %d", game_cfg->home_num);
	}

	//
	// Allocate the home areas.
	//
	_home_num = game_cfg->home_num;

	for (int i = 0; i < _home_num; i++) {
		log_debug("Creating area: %d", i);

		s_area_create(&_home_area[i].area, &game_cfg->drop_dim, &game_cfg->home_size);
	}

	//
	// Allocate backup storage.
	//
	s_point_set(&_blocks_dim, game_cfg->drop_dim.row, game_cfg->drop_dim.col);

	_blocks = blocks_create(_blocks_dim.row, _blocks_dim.col);

	//
	// Reset / initialize the home area.
	//
	home_area_reset(game_cfg);
}

/******************************************************************************
 * The function returns the size of the home area depending on the layout
 * (horizontal / vertical).
 *****************************************************************************/

s_point home_area_get_size(const bool horizontal) {
	s_point result;

	if (horizontal) {
		result.row = _home_area[0].area.dim.row * _home_area[0].area.size.row;
		result.col = _home_num * (_home_area[0].area.dim.col * _home_area[0].area.size.col) + _home_area[0].area.size.col * (_home_num - 1);

	} else {
		result.row = _home_num * (_home_area[0].area.dim.row * _home_area[0].area.size.row) + _home_area[0].area.size.row * (_home_num - 1);
		result.col = _home_area[0].area.dim.col * _home_area[0].area.size.col;

	}

	return result;
}

/******************************************************************************
 * The function assigns a position to every one of the home areas, depending on
 * the upper left position of all home areas and the layout.
 *****************************************************************************/

void home_area_layout(const s_point *pos, const bool horizontal) {

	for (int i = 0; i < _home_num; i++) {

		if (horizontal) {
			_home_area[i].area.pos.row = pos->row;
			_home_area[i].area.pos.col = pos->col + (_home_area[i].area.dim.col * _home_area[i].area.size.col) * i + _home_area[i].area.size.col * i;

		} else {
			_home_area[i].area.pos.row = pos->row + (_home_area[i].area.dim.row * _home_area[i].area.size.row) * i + _home_area[i].area.size.row * i;
			_home_area[i].area.pos.col = pos->col;
		}

		log_debug("home area: %d pos: %d/%d", i, _home_area[i].area.pos.row, _home_area[i].area.pos.col);
	}
}

/******************************************************************************
 * The function returns the next unused home area. It returns true if an area
 * was found. It returns false if there is only one unused home area and this
 * home area is picked up, so there is no other unused home area.
 *****************************************************************************/

bool home_area_next_unused(s_point *pos) {
	int start, end;

	//
	// If no home area is not picked up, we check all home areas.
	//
	if (_pickup_idx == PICKUP_IDX_UNDEF) {
		start = 0;
		end = _home_num;

	}

	//
	// If a home area is picked up, we check the rest of the home areas. We
	// start after the current.
	//
	else {
		start = _pickup_idx + 1;
		end = _pickup_idx + _home_num;
	}

	log_debug("start: %d end: %d num: %d pickup: %d", start, end, _home_num, _pickup_idx);

	for (int idx, i = start; i < end; i++) {

		//
		// Ensure that the index is in range
		//
		idx = i % _home_num;

		//
		//  If we found an unsused home area, we copy the position and return
		// true.
		//
		if (!_home_area[idx].droped) {
			s_point_copy(pos, &_home_area[idx].area.pos);
			return true;
		}
	}

	//
	// At this point, we did not found an unused home area, other than the
	// currently picked up area (if an area is picked up).
	//
	return false;
}