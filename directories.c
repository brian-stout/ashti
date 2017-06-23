#include "directories.h"

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
