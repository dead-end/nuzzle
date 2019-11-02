#ifndef INC_COMMON_H_
#define INC_COMMON_H_

#include <stdio.h>
#include <stdlib.h>

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
#define BLOCK_BOTH L'\u2592'
//#define BLOCK_BOTH L'\u258C'

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

#endif /* INC_COMMON_H_ */
