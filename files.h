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

bool check_dir_exists(const char * path);
bool check_file_exists(const char * filename);
bool detect_path_traversal(char * path);

#endif
