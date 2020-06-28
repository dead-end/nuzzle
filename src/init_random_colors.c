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

#include "s_game_cfg.h"
#include "colors.h"

/******************************************************************************
 * The function initializes the _random value, which will be configured in the
 * game cfg file.
 *****************************************************************************/

static int _random;

void init_random_colors_setup(const char *data) {
	_random = str_2_int(data);

	if (_random < 1 || _random > 100) {
		log_exit("Random value is invalid: %d (allowed: 1 - 100)", _random);
	}

	log_debug("Random: %d", _random);
}

/******************************************************************************
 * The function fills a block array with random colors. The function ensures
 * that the center block has a color.
 *****************************************************************************/

void init_random_colors(const s_game_cfg *game_cfg, t_block **blocks) {

	//
	// Get the center block coordinates.
	//
	const int row_center = (game_cfg->drop_dim.row / 2);
	const int col_center = (game_cfg->drop_dim.col / 2);

	log_debug("center: %d/%d", row_center, col_center);

	//
	// Set the center color
	//
	blocks[row_center][row_center] = colors_random_color();

	for (int row = 0; row < game_cfg->drop_dim.row; row++) {
		for (int col = 0; col < game_cfg->drop_dim.col; col++) {

			//
			// Skip the center block, which is already set.
			//
			if (row == row_center && col == col_center) {
				continue;
			}

			//
			// First check if a block should get a color.
			//
			if (rand() % 100 < _random) {
				blocks[row][col] = CLR_NONE;

			} else {
				blocks[row][col] = colors_random_color();
			}

			log_debug("block: %d/%d color: %d", row, col, blocks[row][col]);
		}
	}
}
