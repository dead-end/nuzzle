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

#include "s_shapes.h"
#include "colors.h"
#include "common.h"

/*******************************************************************************
 * Definition of a fixed size array with shape structures.
 ******************************************************************************/

#define SHAPES_MAX 256

static s_shape _shapes[SHAPES_MAX];

static int _num_shapes;

/*******************************************************************************
 * Definitions of characters for the reading and writing of the shape data.
 ******************************************************************************/

#define SHAPE_READ_DEF 'x'

#define SHAPE_READ_UNDEF ' '

#define SHAPE_WRITE_DEF 'x'

#define SHAPE_WRITE_UNDEF '-'

#define SHAPE_COMMENT '#'

/*******************************************************************************
 * The function is used for logging and prints a shape.
 ******************************************************************************/

#ifdef DEBUG

static void s_shape_debug(const int idx) {
	char str[SHAPE_DIM + 1];

	//
	// Ensure the string termination.
	//
	str[SHAPE_DIM] = '\0';

	log_debug("idx: %d label: %d", idx, _shapes[idx].label);

	for (int i = 0; i < SHAPE_DIM; i++) {

		//
		// Create a line with the row of the shape.
		//
		for (int j = 0; j < SHAPE_DIM; j++) {
			str[j] = _shapes[idx].blocks[i][j] == SHAPE_DEF ? SHAPE_WRITE_DEF : SHAPE_WRITE_UNDEF;
		}

		log_debug(" %s", str);
	}
}

#endif

/*******************************************************************************
 * The function initializes a shape struct.
 ******************************************************************************/

static void s_shape_init(const int idx) {

	//
	// Initialize the label
	//
	_shapes[idx].label = SHAPE_UNDEF;

	//
	// Initialize the blocks
	//
	for (int i = 0; i < SHAPE_DIM; i++) {
		for (int j = 0; j < SHAPE_DIM; j++) {
			_shapes[idx].blocks[i][j] = SHAPE_UNDEF;
		}
	}
}

/*******************************************************************************
 * The function adds a line to a shape struct. It is assumed that the shape is
 * initialized, because the line may be smaller than the dimension.
 ******************************************************************************/

static void s_shape_add_line(const int idx_shape, const int idx_line, const char *line) {

	log_debug("Adding line: '%s'", line);

	//
	// Each character of the line is mapped to a block entry.
	//
	const int end = strlen(line);

	if (end > SHAPE_DIM) {
		log_exit("line too long: '%s'", line);
	}

	for (int i = 0; i < end; i++) {

		if (line[i] == SHAPE_READ_DEF) {
			_shapes[idx_shape].blocks[idx_line][i] = SHAPE_DEF;

		} else if (line[i] != SHAPE_READ_UNDEF) {
			log_exit("Invalid line: '%s'", line);
		}
	}
}

/*******************************************************************************
 * The function is called after the blocks of the shape are copied. It computes
 * the number of blocks which are defined.
 ******************************************************************************/

static void s_shape_finish(const int idx) {

	s_shape *shape = &_shapes[idx];

	shape->label = 0;

	for (int i = 0; i < SHAPE_DIM; i++) {
		for (int j = 0; j < SHAPE_DIM; j++) {

			if (shape->blocks[i][j] == SHAPE_DEF) {
				shape->label++;
			}
		}
	}

#ifdef DEBUG
	s_shape_debug(idx);
#endif
}

/*******************************************************************************
 * The function read the content of the file and fills the shape structures.
 ******************************************************************************/

#define BUF_SIZE 1024

void s_shapes_read(const char *path) {
	char line[1024];
	int idx = -1;
	_num_shapes = 0;

	log_debug("Reading shapes from file: %s", path);

	//
	// Open the score file.
	//
	FILE *file = fopen(path, "r");
	if (file == NULL) {
		log_exit("Unable open file: %s - %s", path, strerror(errno));
	}

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
		// Ignore comments.
		//
		if (line[0] == SHAPE_COMMENT) {
			continue;
		}

		//
		// Remove tailing white spaces (leading white spaces are relevant).
		//
		trim_r(line);

		//
		// Empty line
		//
		if (strlen(line) == 0) {

			//
			// If the index is >=0, we are inside a block of the shape, so we
			// have to finish it.
			//
			if (idx >= 0) {
				idx = -1;
				s_shape_finish(_num_shapes - 1);
			}
			continue;
		}

		//
		// If the index is -1, we are outside the block, the line is empty, so
		// we have a new block.
		//
		if (idx == -1) {

			//
			// Ensure that there is at least one shape unused.
			//
			if (_num_shapes >= SHAPES_MAX) {
				log_exit("Too many shapes - max: %d", SHAPES_MAX);
			}

			//
			// Increase the number of shapes.
			//
			_num_shapes++;

			//
			// Initialize the shape.
			//
			s_shape_init(_num_shapes - 1);
		}

		idx++;

		s_shape_add_line(_num_shapes - 1, idx, line);

	}

	//
	// At this point we are at EOF. Ensure that there is not an unfinished
	// shape.
	//
	if (idx >= 0) {
		s_shape_finish(_num_shapes - 1);
	}

	fclose(file);
}

/*******************************************************************************
 * The function copies a random shape to an area. The shape structure has a
 * fixed size / dimension. The target area may be smaller.
 ******************************************************************************/

void s_shapes_init_random(t_block **blocks, const int rows, const int cols) {

#ifdef DEBUG

	//
	// Ensure that the dimension is valid.
	//
	if (rows < 0 || rows > SHAPE_DIM || cols < 0 || cols > SHAPE_DIM) {
		log_exit("Invalid dim: %d/%d", rows, cols);
	}
#endif

	//
	// Select a random shape.
	//
	const int idx = rand() % _num_shapes;
	const s_shape *shape = &_shapes[idx];

	log_debug("Selecting shape: %d", idx);

	//
	// Copy the shape.
	//
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			blocks[i][j] = shape->blocks[i][j] == SHAPE_DEF ? CLR_MK_N : CLR_NONE;
		}
	}
}
