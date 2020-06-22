/*
 * MIT License
 *
 * Copyright (c) 2019 dead-end
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

#ifndef INC_COMMON_H_
#define INC_COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define VERSION "0.2.0"

/******************************************************************************
 * Definition of the print_debug macro. It is only defined if the DEBUG flag is
 * defined. It prints to stderr not to restrain curses.
 *****************************************************************************/

#ifdef DEBUG

#define log_debug(fmt, ...) fprintf(stderr, "DEBUG %s:%d:%s() " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define log_debug_str(fmt)  fprintf(stderr, "DEBUG %s:%d:%s() " fmt "\n", __FILE__, __LINE__, __func__)

#define DEBUG_USED

#else

#define log_debug(fmt, ...)
#define log_debug_str(fmt)

#define DEBUG_USED __attribute__((unused))

#endif

/******************************************************************************
 * Definition of the print_error macro, that finishes the program after
 * printing the error message.
 *****************************************************************************/

#define log_exit(fmt, ...) fprintf(stderr, "FATAL %s:%d:%s() " fmt "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); exit(EXIT_FAILURE)
#define log_exit_str(fmt)  fprintf(stderr, "FATAL %s:%d:%s() " fmt "\n", __FILE__, __LINE__, __func__); exit(EXIT_FAILURE)

void* xmalloc(const size_t size);

// -------------------------------

#define BLOCK_FULL L'\u2588'

#define BLOCK_EMPTY L' '

// TODO: choose correct character

//#define BLOCK_BOTH L'\u2593'

//#define BLOCK_BOTH L'\u258C'

#define BLOCK_BOTH L'\u2592'

#define KEY_ESC 27

/******************************************************************************
 * The s_point struct represents an element that has a row and a column. This
 * can be a pixel (terminal character), an array dimension, a block size...
 *****************************************************************************/

typedef struct s_point {

	int row;

	int col;

} s_point;

//
// The macro sets the row and column of a s_point struct.
//
#define s_point_set(p,r,c) (p)->row = (r);(p)->col = (c)

#define s_point_copy(t,f) (t)->row = (f)->row; (t)->col = (f)->col

#define s_point_same(t,f) (((t)->row == (f)->row) && ((t)->col == (f)->col))

#define max(a,b) ((a) > (b) ? (a) : (b))

#define bool_str(b) (b) ? "true" : "false"

#define center(t,w) (((t) - (w)) / 2)

/******************************************************************************
 *
 *****************************************************************************/

s_point strs_dim(const char *strs[]);

void trim_r(char *str);

int str_2_int(const char *str);

char* cpy_str_centered(char *to, const int size, const char *from);

#endif /* INC_COMMON_H_ */
