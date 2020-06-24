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

#include <errno.h>
#include <sys/stat.h>

#include "common.h"

/*******************************************************************************
 * The function checks whether a system file entry exists or not. The entry can
 * be a file (true) or a directory (false).
 ******************************************************************************/

bool fs_entry_exists(const char *path, const bool reg_file) {
	struct stat sb;

	const int stat_result = stat(path, &sb);

	//
	// Stat returns an error if no file system entry exists.
	//
	if (stat_result == -1) {

		//
		// Ensure that there is no other error (example: ELOOP Too many symbolic
		// links)
		//
		if (errno == ENOENT) {
			log_debug("Path: %s errno: %s", path, strerror(errno));
			return false;
		}

		log_exit("Unable to check directory: %s", strerror(errno));
	}

	//
	// The file system exists, so we have to check whether it is a file.
	//
	else if (reg_file) {
		return S_ISREG(sb.st_mode);
	}

	//
	// The file system exists, so we have to check whether it is a directory.
	//
	else {
		return S_ISDIR(sb.st_mode);
	}
}
