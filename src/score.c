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
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "s_status.h"
#include "file_system.h"

/*******************************************************************************
 * The definitions of the nuzzle directories and files.
 ******************************************************************************/

#define NUZZLE_DIR ".nuzzle"

/******************************************************************************
 * The function checks if the nuzzle directory exists. If not, it will be
 * created.
 *****************************************************************************/

void fs_ensure_nuzzle_dir() {

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
	char path[PATH_MAX];

	if (snprintf(path, PATH_MAX, "%s/%s", homedir, NUZZLE_DIR) >= PATH_MAX) {
		log_exit_str("Path is too long!");
	}

	//
	// If the directory does not exist, we create it.
	//
	if (!fs_entry_exists(path, CHECK_DIR) && mkdir(path, S_IRWXU | S_IRWXG) == -1) {
		log_exit("Unable to create directory: %s - %s", path, strerror(errno));
	}
}

/******************************************************************************
 * The function creates the filename for of the score file.
 *****************************************************************************/

static void get_score_file(const s_status *status, char *path, const int buf_len) {
	const char *homedir;

	if ((homedir = getenv("HOME")) == NULL) {
		log_exit_str("Home directory not found!");
	}

	if (snprintf(path, buf_len, "%s/%s/score-id-%d", homedir, NUZZLE_DIR, status->game_cfg->id) >= buf_len) {
		log_exit_str("Path is too long!");
	}
}

/*******************************************************************************
 * The function reads the score from the score file. If the score file does not
 * exist, the method returns 0.
 ******************************************************************************/

int score_read(const s_status *status) {
	char path[PATH_MAX];

	//
	// Get the score file path.
	//
	get_score_file(status, path, PATH_MAX);

	//
	// If the score file does not exist, we return 0.
	//
	if (!fs_entry_exists(path, CHECK_FILE)) {
		log_debug("File does not exist: %s", path);
		return 0;
	}

	//
	// Open the score file.
	//
	FILE *file = fopen(path, "r");
	if (file == NULL) {
		log_exit("Unable open file: %s - %s", path, strerror(errno));
	}

	//
	// Read the score
	//
	int result;
	if (fscanf(file, "%d", &result) != 1) {
		log_exit("Unable parse file: %s", path);
	}

	fclose(file);

	//
	// Return the score
	//
	log_debug("Read score: %d from: %s", result, path);
	return result;
}

/*******************************************************************************
 * The function writes the score to the score file. If the nuzzle directory,
 * which contains the score file does not exist, it will be created.
 ******************************************************************************/

void score_write(const s_status *status, const int score) {
	char path[PATH_MAX];

	//
	// Ensure that the nuzzle directory exists.
	//
	fs_ensure_nuzzle_dir();

	//
	// Get the absolute path of the score file.
	//
	get_score_file(status, path, PATH_MAX);

	//
	// Open the score file.
	//
	FILE *file = fopen(path, "w");
	if (file == NULL) {
		log_exit("Unable open file: %s - %s", path, strerror(errno));
	}

	//
	// Write the score to the score file.
	//
	fprintf(file, "%d", score);

	fclose(file);
}
