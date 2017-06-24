#ifndef FILES_H
#define FILES_H
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

#include <sys/types.h>
#include <dirent.h>
#include <stdbool.h>
#include <sysexits.h>
#include <sys/stat.h>
#include <string.h>

/** check_dir_exists() is a quick function to check if a directory is valid
*/
bool check_dir_exists(const char * path);

/** check_file_exists() is a quick function to check if a file is there and can be read
*/
bool check_file_exists(const char * filename);

/** detect_path_traversal() is a quick function that blacklists any bizarre request that
*       appear to be circumventing the working directory of the server using path traversal
*       EX: /../../../ashti.c
*/
bool detect_path_traversal(char * path);

#endif
