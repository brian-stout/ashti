#include "files.h"

bool check_file_exists(const char * filename)
{
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL) {
        return false;
    } else {
        fclose(fp);
        return true;
    }
}

bool check_dir_exists(const char * path)
{
        //https://stackoverflow.com/a/12510903
        errno = 0;
        DIR* dir = opendir(path);
        //Directory does exist
        if(dir) {
            closedir(dir);
            return true;
        }
        //Directory does not exist
        else if(ENOENT == errno) {
            return false;
        }
        //opendir failed for some other reason
        else {
            return false;
        }
}

bool detect_path_traversal(char * path)
{
    if(path[0] == '.') {
        return false;
    }
    if(path[0] == '$') {
        return false;
    }
    if(path[0] == 'C' && path[1] ==':') {
        return false;
    }

    size_t len = strlen(path);

    int dot_count = 0;
    for(size_t i = 0; i < len; i++) {
        if(path[i] == '.') {
            dot_count++;
        } else {
            dot_count = 0;
        }
        if(dot_count > 1) {
            return false;
        }
    }

    return true;
}
