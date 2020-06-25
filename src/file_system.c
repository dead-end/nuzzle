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

#include <linux/limits.h>
#include <errno.h>
#include <sys/stat.h>

#include "common.h"
#include "file_system.h"

/*******************************************************************************
 * The definitions of the nuzzle directories and files.
 ******************************************************************************/

#define CFG_DIR_HOME ".nuzzle"

#define CFG_DIR_REL "cfg"

#define CFG_DIR_SYS "/etc/nuzzle"

/*******************************************************************************
 * The function checks whether a system file entry exists or not. The entry can
 * be a file (true) or a directory (false).
 *
 * (unit tested)
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

/******************************************************************************
 * The function creates the path of the nuzzle directory, which is:
 *
 *   <HOME>/.nuzzle
 *****************************************************************************/

void fs_nuzzle_dir_get(char *path, const int size) {

	//
	// Get the home directory.
	//
	const char *homedir;

	if ((homedir = getenv("HOME")) == NULL) {
		log_exit_str("Home directory not found!");
	}

	//
	// Get the nuzzle directory.
	//
	if (snprintf(path, size, "%s/%s", homedir, CFG_DIR_HOME) >= size) {
		log_exit_str("Path is too long!");
	}
}

/******************************************************************************
 * The function checks if the nuzzle directory exists. If not, it will be
 * created.
 *****************************************************************************/

void fs_nuzzle_dir_ensure() {
	char path[PATH_MAX];

	//
	// Get the nuzzle directory.
	//
	fs_nuzzle_dir_get(path, PATH_MAX);

	//
	// If the directory does not exist, we create it.
	//
	if (!fs_entry_exists(path, CHECK_DIR) && mkdir(path, S_IRWXU | S_IRWXG) == -1) {
		log_exit("Unable to create directory: %s - %s", path, strerror(errno));
	}
}

/*******************************************************************************
 * The function checks the nuzzle configuration directories for a given file.
 * The function returns true if the file was found in one of the directories and
 * copies the path to the path parameter and returns true. The function returns
 * false if the file was not found.
 ******************************************************************************/

bool fs_get_cfg_file(const char *name, char *path, const int size) {

	//
	// check: cfg/<file>
	//
	if (snprintf(path, size, "%s/%s", CFG_DIR_REL, name) >= size) {
		log_exit("Truncated: %s/%s", CFG_DIR_REL, name);
	}

	if (fs_entry_exists(path, CHECK_FILE)) {
		log_debug("File exists: %s", path);
		return true;
	}

	//
	// check: <home>/.nuzzle/<file>
	//
	char tmp[PATH_MAX];
	fs_nuzzle_dir_get(tmp, PATH_MAX);

	if (snprintf(path, size, "%s/%s", tmp, name) >= size) {
		log_exit("Truncated: %s/%s", tmp, name);
	}

	if (fs_entry_exists(path, CHECK_FILE)) {
		log_debug("File exists: %s", path);
		return true;
	}

	//
	// check: /etc/nuzzle/<file>
	//
	if (snprintf(path, size, "%s/%s", CFG_DIR_SYS, name) >= size) {
		log_exit("Truncated: %s/%s", CFG_DIR_SYS, name);
	}

	if (fs_entry_exists(path, CHECK_FILE)) {
		log_debug("File exists: %s", path);
		return true;
	}

	//
	// At this point we did not found a matching file.
	//
	return false;
}
