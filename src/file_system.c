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
#include "file_system.h"

/*******************************************************************************
 * The array defines the lookup paths for configuration files. The order of the
 * array matters.
 ******************************************************************************/

const char *fs_cfg_dirs[] = { "cfg", "/etc/nuzzle", NULL };

/*******************************************************************************
 * The function checks whether a system file entry exists or not. The entry can
 * be a file (true) or a directory (false).
 ******************************************************************************/
// TODO: unit test
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

/*******************************************************************************
 * The function is called with a NULL terminated array of directories and a file
 * name. It concatenates each directory with the file name and check if the
 * result exists in the file system. It saves the result in the path parameter
 * and returns true if something was found.
 ******************************************************************************/
// TODO: unit test
bool fs_get_cfg_file(const char *dirs[], const char *name, char *path, const int size) {

	//
	// Iterate over the NULL terminates array of directory paths
	//
	for (const char **ptr = dirs; *ptr != NULL; ptr++) {

		//
		// Concatenate the directory with the file name
		//
		if (snprintf(path, size, "%s/%s", *ptr, name) >= size) {
			log_exit("Truncated: %s/%s", *ptr, name);
		}

		//
		// If the result exists (and is a file) we are done.
		//
		if (fs_entry_exists(path, CHECK_FILE)) {
			log_debug("File exists: %s", path);
			return true;
		}
	}

	//
	// At this point we did not found a matching file.
	//
	return false;
}
