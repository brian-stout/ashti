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

FILE * GET_response(char * request, bool * cgi_bin_request)
{
    FILE * fp;
    char buf[512];

    if(request[0] == '/' && request[1] == '\0') {
        fp = fopen("www/index.html", "r");
        if(fp) {
            return fp;
        }
        else {
            return NULL;
        }
    } else {
        if(strncmp("cgi-bin", request+1, 7) == 0){
            *cgi_bin_request = true;
        } else {
            snprintf(buf, sizeof(buf), "www%s", request);
            fp = fopen(buf, "r");
        }
        return fp;
    }
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

