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

#include "ut_utils.h"
#include "file_system.h"

/******************************************************************************
 * The function tests the fs_entry_exists() function.
 *****************************************************************************/

static void test_fs_entry_exists() {
	bool result;

	//
	// Check dir
	//
	result = fs_entry_exists("/etc", false);
	ut_check_bool(result, true, "directory exists");

	result = fs_entry_exists("/etc2", false);
	ut_check_bool(result, false, "directory does not exist");

	result = fs_entry_exists("/etc/passwd", false);
	ut_check_bool(result, false, "directory is a file");

	//
	// Check file
	//
	result = fs_entry_exists("/etc/passwd", true);
	ut_check_bool(result, true, "file exists");

	result = fs_entry_exists("/etc/passwd2", true);
	ut_check_bool(result, false, "file does not exists");

	result = fs_entry_exists("/etc", true);
	ut_check_bool(result, false, "file is a directory");
}

/******************************************************************************
 * The function is the a wrapper, that triggers the internal unit tests.
 *****************************************************************************/

void ut_file_system_exec() {

	test_fs_entry_exists();
}
