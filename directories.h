#ifndef DIRECTORIES_H
#define DIRECTORIES_H
#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

#include <sys/types.h>
#include <dirent.h>
#include <stdbool.h>
#include <sysexits.h>

bool check_dir_exists(const char * path);
bool check_file_exists(const char * filename);

#endif
