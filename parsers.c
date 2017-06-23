#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sysexits.h>

#include <limits.h>
#include <errno.h>

#include <dirent.h>
#include <sysexits.h>

const char * header_code[] = { "OPTIONS", "GET", "HEAD", "POST",
                                "PUT", "DELETE", "TRACE", "CONNECT", NULL};

char request[] = "GET / HTTP/1.1 Host: tooldev:3012 User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0 Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8 Accept-Language: en-US,en;q=0.5 Accept-Encoding: gzip, deflate Connection: keep-alive Upgrade-Insecure-Requests: 1";

char page_request[] = "GET /index.html HTTP/1.1 Host: tooldev:3012 User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:52.0) Gecko/20100101 Firefox/52.0 Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8 Accept-Language: en-US,en;q=0.5 Accept-Encoding: gzip, deflate Connection: keep-alive Upgrade-Insecure-Requests: 1";

const char * www_root_missing = "ERROR : www_root does not exist in the"
                                " folder where the binary is located,"
                                " or in the directory specified from a"
                                " command line argument.  Either move\n"
                                " the binary to a folder containing www_root"
                                " or specify a path to a custom root directory.\n"
                                "USAGE: ./ashti *<path to www_root>\n";

const char * directory_is_missing = "ERROR : the specified directory does"
                                    " not exist.\n"
                                    "USAGE: ./ashti <path to www_root>\n";

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

int determine_request(char * request)
{
    for(int i = 0; header_code[i] != NULL; i++)
    {
        int cmp = 0;
        cmp = strncmp(header_code[i], request, strlen(request));
        if (cmp == 0) {
            return i;
        } 
    }
    return -1;
}

FILE * GET_response(char * request)
{
    FILE * fp;

    if(request[0] == '/' && request[1] == '\0') {
        printf("Look for index.html\n");
        fp = fopen("www/index.html", "r");
        if(fp) {
            return fp;
        }
        else {
            return NULL;
        }
    } else {
        printf("%s\n", request);
        return NULL;
    }
}

int main(int argc, char *argv[])
{
    errno = 0;
    char directory[256];

    if (argc == 1) {
        if (check_dir_exists("www_root") == false) {
            fprintf(stderr, "%s\n", www_root_missing);
            return EX_USAGE;
        }
        chdir("www_root");
    } else if (argc == 2) {
        //TODO: Determine if absolute path or relative
        //TODO: Look for www_root in binary's directory
        if (check_dir_exists(argv[1]) == false) {
            fprintf(stderr, "%s\n", directory_is_missing);
            return EX_USAGE;
        } else {
            chdir(argv[1]);
        }
    } else {
        //TODO: Error handle
        fprintf(stderr, "An unknown error occured : %d\n", errno);
        return EX_OSERR;
    }

    getcwd(directory, sizeof(directory));
    if (directory == NULL) {
        fprintf(stderr, "Could not get current working directory error: %d\n", errno);
        return EX_OSERR;
    }
    printf("Server Directory: %s\n", directory);
    char *token;

    token = strtok(request, " ");
    int switch_int = determine_request(token);
    FILE * fp;
    switch(switch_int) {
        case 1:
            printf("GET COMMAND\n");
            token = strtok(NULL, " ");
            fp = GET_response(token);
            break;
        default:
            printf("505 Error?\n");
    }

/* STEPS
    1. Get code for what request it is, return to a switch
    2. Switch runs proper commands, token needs to get passed so it can continue down
        the request

    GET request
    1. Process GET Command, get token of file it wants to GET
    2. Determine if that file exists in CGI_BIN or WWW
    3. If it does not return 404
    4. If it does return a file pointer
    5. Load file pointer binary into an array
    6. Send all data back to the browser
    7. TODO: If in CGI-BIN run script, read the stdout into an array
    8.  Send that garbage back to the browser
*/
    
}

