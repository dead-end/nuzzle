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

#ifndef INC_FILE_SYSTEM_H_
#define INC_FILE_SYSTEM_H_

#include <stdbool.h>

/*******************************************************************************
 * The definitions of the nuzzle directories and files.
 ******************************************************************************/

#ifdef PREFIX
#define NUZZLE_CFG_DIR_SYS PREFIX "/share/games/nuzzle"

#else
#define NUZZLE_CFG_DIR_SYS "/usr/local/share/games/nuzzle"
#endif

#define NUZZLE_CFG_DIR_HOME ".nuzzle"

#define NUZZLE_CFG_DIR_REL "cfg"

#define NUZZLE_CFG_FILE "nuzzle.cfg"

/*******************************************************************************
 * Two definitions for a flag makes it more readable.
 ******************************************************************************/

#define CHECK_FILE true

#define CHECK_DIR false

/*******************************************************************************
 * Definition of the functions.
 ******************************************************************************/

bool fs_entry_exists(const char *path, const bool reg_file);

void fs_nuzzle_dir_get(char *path, const int size);

void fs_nuzzle_dir_ensure();

bool fs_get_cfg_file(const char *name, char *path, const int size);

#endif /* INC_FILE_SYSTEM_H_ */
