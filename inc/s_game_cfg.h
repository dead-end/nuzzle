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

#ifndef INC_S_GAME_CFG_H_
#define INC_S_GAME_CFG_H_

#include "colors.h"
#include "common.h"

/******************************************************************************
 * The definition of the game types.
 *****************************************************************************/

#define TYPE_UNDEF -1

#define TYPE_4_COLORS 1

#define TYPE_SQUARES_LINES 2

//
// String values for the configuration file.
//
#define TYPE_4_COLORS_STR "4-colors"

#define TYPE_SQUARES_LINES_STR "squares-lines"

/******************************************************************************
 * The definition of the s_game struct.
 *****************************************************************************/

#define SIZE_TITLE 32

#define SIZE_DATA 1024

typedef struct s_game_cfg {

	// TODO: not used
	int id;

	//
	// An parameter with the type of the game.
	//
	int type;

	//
	// A container for game specific data (file names, probability,...)
	//
	char data[SIZE_DATA];

	//
	// A string containing the title of the game.
	//
	char title[SIZE_TITLE];

	//
	// The dimension of the game.
	//
	s_point game_dim;

	//
	// The dimenstion of the drop area and the home areas.
	//
	s_point drop_dim;

	//
	// The number of home areas.
	//
	int home_num;

	//
	// An enum with the different chess types for the background of the areas.
	//
	e_chess_type chess_type;

} s_game_cfg;

/******************************************************************************
 * Declaration of an array for game configurations.
 *****************************************************************************/

#define S_GAMES_CFG_MAX 4

extern int s_game_cfg_num;

extern s_game_cfg game_cfg[S_GAMES_CFG_MAX];

/******************************************************************************
 * Function definitions.
 *****************************************************************************/

void s_game_cfg_read(const char *path);

#endif /* INC_S_GAME_CFG_H_ */
