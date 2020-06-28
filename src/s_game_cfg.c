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

#include <stdio.h>
#include <linux/limits.h>
#include <errno.h>

#include "s_game_cfg.h"

#include "colors.h"
#include "file_system.h"
#include "rules.h"

/*******************************************************************************
 * Forward declaration of the functions for the function pointers to prevent
 * cyclic includes.
 *
 * The s_shapes file has to include "s_game_cfg.h", to be able to use the
 * s_game_cfg struct.
 ******************************************************************************/

void s_shapes_read(const char *path);

void s_shapes_init_random(const s_game_cfg *game_cfg, t_block **blocks);

void init_random_colors_setup(const char *data);

void init_random_colors(const s_game_cfg *game_cfg, t_block **blocks);

/*******************************************************************************
 * Declaration of an array for game configurations.
 ******************************************************************************/

int s_game_cfg_num = 0;

s_game_cfg _game_cfg[S_GAMES_CFG_MAX];

/*******************************************************************************
 * The definition of keys and tags for the configuration file.
 ******************************************************************************/

#define CFG_GAME_TAG "[game]"

#define CFG_GAME_ID "game.id"

#define CFG_GAME_TITLE "game.title"

#define CFG_GAME_TYPE "game.type"

#define CFG_GAME_TYPE_DATA "game.data"

#define CFG_GAME_DIM_ROW "game.dim.row"

#define CFG_GAME_DIM_COL "game.dim.col"

#define CFG_GAME_SIZE_ROW "game.size.row"

#define CFG_GAME_SIZE_COL "game.size.col"

#define CFG_DROP_DIM_ROW "drop.dim.row"

#define CFG_DROP_DIM_COL "drop.dim.col"

#define CFG_HOME_NUM "home.num"

#define CFG_HOME_SIZE_ROW "home.size.row"

#define CFG_HOME_SIZE_COL "home.size.col"

#define CFG_COLOR "color"

/*******************************************************************************
 * The macro checks if the string starts with a prefix.
 ******************************************************************************/

#define starts_with(s,p) (strncmp(p, s, strlen(p)) == 0)

/*******************************************************************************
 * The function is called with a key value pair ("key=value") and returns a
 * pointer to the value.
 ******************************************************************************/

static char* cfg_get_value(const char *line) {

	//
	// Get a pointer to the equals sign.
	//
	char *result = index(line, '=');

	//
	// Ensure that the string contains a '='.
	//
	if (result == NULL) {
		log_exit("No value found in line: %s", line);
	}

	//
	// Skip the '=' and return the result.
	//
	return ++result;
}

/*******************************************************************************
 * The function is called with a key value pair ("key=value") and copies the
 * value to an array with a given size.
 ******************************************************************************/

static inline void cfg_get_str(char *to, const char *line, const size_t size) {
	const char *value = cfg_get_value(line);

	if (strlen(value) > size - 1) {
		log_exit("Value too long: %s", value);
	}

	strncpy(to, cfg_get_value(line), size);
}

/*******************************************************************************
 * The function is called with a key value pair ("key=value") and returns the
 * type of the game, which is translated from a string to an integer value.
 ******************************************************************************/

static int cfg_get_type(const char *line) {
	const char *value = cfg_get_value(line);

	if (strcmp(TYPE_LINES_STR, value) == 0) {
		return TYPE_LINES;

	} else if (strcmp(TYPE_SQUARES_LINES_STR, value) == 0) {
		return TYPE_SQUARES_LINES;

	} else if (strcmp(TYPE_4_COLORS_STR, value) == 0) {
		return TYPE_4_COLORS;

	} else {
		log_exit("Unknown type: %s", line);
	}
}

/*******************************************************************************
 * The function parses a color from the line. Valid values are:
 *
 * - Games: "lines" and "squares-lines": red, green, blue, yellow
 *
 * - Games: "4-colors": empty string
 ******************************************************************************/

static int cfg_get_color(const int type, const char *line) {

	if (type == TYPE_UNDEF) {
		log_exit("Type undefined: %s", line);
	}

	const char *value = cfg_get_value(line);

	if (type == TYPE_4_COLORS) {

		if (strlen(value) == 0) {
			return CLR_NONE;
		}

		log_exit("Invalid color definition for 4-colors: %s", line);

	} else {

		if (strlen(value) == 0) {
			return CLR_BLUE_N;
		}

		if (strcmp(value, "red") == 0) {
			return CLR_RED__N;
		}

		if (strcmp(value, "green") == 0) {
			return CLR_GREE_N;
		}

		if (strcmp(value, "blue") == 0) {
			return CLR_BLUE_N;
		}

		if (strcmp(value, "yellow") == 0) {
			return CLR_YELL_N;
		}

		log_exit("Unknown color: %s", line);
	}
}

/*******************************************************************************
 * The macro is called with a key value pair ("key=value") and it is assumed
 * that the value is an integer. It returns the value as an int.
 ******************************************************************************/

#define cfg_get_int(l) str_2_int(cfg_get_value(l))

/*******************************************************************************
 * The function prints a game config structure. With non debug mode, the
 * function is empty and produces unused-parameter warnings.
 ******************************************************************************/

#ifdef DEBUG

void s_game_debug(const s_game_cfg *game_cfg) {

	log_debug("id: %d", game_cfg->id);

	log_debug("title: %s", game_cfg->title);
	log_debug("type: %d", game_cfg->type);

	log_debug("data: %s", game_cfg->data);

	log_debug("game dim: %d/%d", game_cfg->game_dim.row, game_cfg->game_dim.col);
	log_debug("game size: %d/%d", game_cfg->game_size.row, game_cfg->game_size.col);

	log_debug("drop dim: %d/%d", game_cfg->drop_dim.row, game_cfg->drop_dim.col);

	log_debug("home num: %d", game_cfg->home_num);
	log_debug("home size: %d/%d", game_cfg->home_size.row, game_cfg->home_size.col);

	log_debug("color: %d", game_cfg->color);

	log_debug("chess type: %d", game_cfg->chess_type);
}

#endif

/*******************************************************************************
 * The function initializes all values for the s_game structures with invalid
 * values. All invalid values have to be overwritten by the configuration file,
 * so we can check if the configuration file is complete.
 ******************************************************************************/

static void s_game_init() {

	for (int i = 0; i < S_GAMES_CFG_MAX; i++) {

		_game_cfg[i].id = -1;

		_game_cfg[i].title[0] = '\0';

		_game_cfg[i].type = TYPE_UNDEF;

		_game_cfg[i].data[0] = '\0';

		s_point_set(&_game_cfg[i].game_dim, -1, -1);

		s_point_set(&_game_cfg[i].game_size, -1, -1);

		s_point_set(&_game_cfg[i].drop_dim, -1, -1);

		_game_cfg[i].home_num = -1;

		_game_cfg[i].color = -1;

		s_point_set(&_game_cfg[i].home_size, -1, -1);
	}
}

/*******************************************************************************
 * The function checks if all values of the game structure are valid.
 ******************************************************************************/

static void s_game_check() {

	//
	// Ensure that we have at least one game configuration.
	//
	if (s_game_cfg_num <= 0) {
		log_exit_str("No games defined!");
	}

	//
	// We check only the defined s_game structures.
	//
	for (int i = 0; i < s_game_cfg_num; i++) {

		if (_game_cfg[i].id < 0) {
			log_exit("Game: %d -not set: '%s'", i, CFG_GAME_ID);
		}

		if (_game_cfg[i].title[0] == '\0') {
			log_exit("Game: %d - not set: '%s'", i, CFG_GAME_TITLE);
		}

		if (_game_cfg[i].type == TYPE_UNDEF) {
			log_exit("Game: %d -not set: '%s'", i, CFG_GAME_TYPE);
		}

		if (_game_cfg[i].data[0] == '\0') {
			log_exit("Game: %d - not set: '%s'", i, CFG_GAME_TYPE_DATA);
		}

		if (_game_cfg[i].game_dim.row < 0 || _game_cfg[i].game_dim.col < 0) {
			log_exit("Game: %d - not set: '%s' or '%s'", i, CFG_GAME_DIM_ROW, CFG_GAME_DIM_COL);
		}

		if (_game_cfg[i].game_size.row < 0 || _game_cfg[i].game_size.col < 0) {
			log_exit("Game: %d - not set: '%s' or '%s'", i, CFG_GAME_SIZE_ROW, CFG_GAME_SIZE_COL);
		}

		if (_game_cfg[i].drop_dim.row < 0 || _game_cfg[i].drop_dim.col < 0) {
			log_exit("Game: %d - not set: '%s' or '%s'", i, CFG_DROP_DIM_ROW, CFG_DROP_DIM_COL);
		}

		if (_game_cfg[i].home_num < 0) {
			log_exit("Game: %d - not set: '%s'", i, CFG_HOME_NUM);
		}

		if (_game_cfg[i].home_size.row < 0 || _game_cfg[i].home_size.col < 0) {
			log_exit("Game: %d - not set: '%s' or '%s'", i, CFG_HOME_SIZE_ROW, CFG_HOME_SIZE_COL);
		}

		if (_game_cfg[i].color < 0) {
			log_exit("Game: %d - not set: '%s'", i, CFG_COLOR);
		}
	}
}

/*******************************************************************************
 * The function processes the configurations from the config file line by line.
 ******************************************************************************/

#define BUF_SIZE 1024

static void s_game_cfg_process(FILE *file, const char *path) {
	char line[BUF_SIZE];
	s_game_cfg *game = NULL;

	//
	// Read the file line by line.
	//
	while (fgets(line, BUF_SIZE, file) != NULL) {

		//
		// Ensure that the IO operation succeeded.
		//
		if (ferror(file)) {
			log_exit("Unable to read file: %s - %s", path, strerror(errno));
		}

		//
		// Remove tailing white spaces.
		//
		trim_r(line);

		//
		// Ignore empty lines and comments.
		//
		if (strlen(line) == 0 || line[0] == '#') {
			continue;
		}

		log_debug("line: %s", line);

		//
		// The game tag signals the start of a new game configuration.
		//
		if (starts_with(line, CFG_GAME_TAG)) {

			if (s_game_cfg_num >= S_GAMES_CFG_MAX) {
				log_exit_str("Too many games!");
			}

			s_game_cfg_num++;
			game = &_game_cfg[s_game_cfg_num - 1];
			continue;
		}

		//
		// Ensure that game is defined, to be able to set game data.
		//
		if (game != NULL) {

			if (starts_with(line, CFG_GAME_ID)) {
				game->id = cfg_get_int(line);

			} else if (starts_with(line, CFG_GAME_TITLE)) {
				cfg_get_str(game->title, line, SIZE_TITLE);

			} else if (starts_with(line, CFG_GAME_TYPE)) {
				game->type = cfg_get_type(line);

				switch (game->type) {

				case TYPE_LINES:
					game->chess_type = CHESS_SIMPLE_LIGHT;
					game->fct_ptr_set_data = s_shapes_read;
					game->fct_ptr_rules_remove = rules_remove_lines;
					game->fct_ptr_init_random = s_shapes_init_random;
					break;

				case TYPE_SQUARES_LINES:
					game->chess_type = CHESS_DOUBLE;
					game->fct_ptr_set_data = s_shapes_read;
					game->fct_ptr_rules_remove = rules_remove_squares_lines;
					game->fct_ptr_init_random = s_shapes_init_random;
					break;

				case TYPE_4_COLORS:
					game->chess_type = CHESS_SIMPLE_LIGHT;
					game->fct_ptr_set_data = init_random_colors_setup;
					game->fct_ptr_rules_remove = rules_remove_neighbors;
					game->fct_ptr_init_random = init_random_colors;
					break;

				default:
					log_debug("Unknown type: %d", game->type);
				}

			} else if (starts_with(line, CFG_GAME_TYPE_DATA)) {
				cfg_get_str(game->data, line, SIZE_DATA);

			} else if (starts_with(line, CFG_GAME_DIM_ROW)) {
				game->game_dim.row = cfg_get_int(line);

			} else if (starts_with(line, CFG_GAME_DIM_COL)) {
				game->game_dim.col = cfg_get_int(line);

			} else if (starts_with(line, CFG_GAME_SIZE_ROW)) {
				game->game_size.row = cfg_get_int(line);

			} else if (starts_with(line, CFG_GAME_SIZE_COL)) {
				game->game_size.col = cfg_get_int(line);

			} else if (starts_with(line, CFG_DROP_DIM_ROW)) {
				game->drop_dim.row = cfg_get_int(line);

			} else if (starts_with(line, CFG_DROP_DIM_COL)) {
				game->drop_dim.col = cfg_get_int(line);

			} else if (starts_with(line, CFG_HOME_NUM)) {
				game->home_num = cfg_get_int(line);

			} else if (starts_with(line, CFG_HOME_SIZE_ROW)) {
				game->home_size.row = cfg_get_int(line);

			} else if (starts_with(line, CFG_HOME_SIZE_COL)) {
				game->home_size.col = cfg_get_int(line);

			} else if (starts_with(line, CFG_COLOR)) {
				game->color = cfg_get_color(game->type, line);

			} else {
				log_exit("Unknown definition: %s", line);
			}

		} else {
			log_exit("Unknown definition: %s", line);
		}
	}
}

/*******************************************************************************
 * The function reads the configurations from the config file to an array of
 * s_game_cfg structures.
 ******************************************************************************/

void s_game_cfg_read(const char *file_name) {

	//
	//  The function is called with the file name. We need the path of the file.
	//
	char path[PATH_MAX];

	if (!fs_get_cfg_file(file_name, path, PATH_MAX)) {
		log_exit("No config file found: %s", file_name);
	}

	//
	// Initialize the game configurations. This is required by the
	// s_game_check() function to be able to check if all configurations are
	// complete
	//
	s_game_init();

	//
	// Open the game configuration file.
	//
	FILE *file = fopen(path, "r");
	if (file == NULL) {
		log_exit("Unable open file: %s - %s", path, strerror(errno));
	}

	//
	// Delegate the processing to a separate function.
	//
	s_game_cfg_process(file, path);

	//
	// Close the file and check for errors.
	//
	if (fclose(file) == -1) {
		log_exit("Unable close file: %s - %s", path, strerror(errno));
	}

#ifdef DEBUG

	for (int i = 0; i < s_game_cfg_num; i++) {
		s_game_debug(&_game_cfg[i]);
	}
#endif

	//
	// Ensure that all game configurations are complete.
	//
	s_game_check();
}
