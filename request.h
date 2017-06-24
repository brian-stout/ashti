#ifndef REQUEST_H
#define REQUEST_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/types.h>

#include "files.h"

/** get_request() processes a GET http request from a browser/client and determines if
*       it is a CGI-BIN request, a home page request, or needs to be looked up in the www
*       folder.
*/
FILE * get_request(char * request, bool * cgi_bin_request);

/** determine_request() is a quick function that compares the strings from the request to an
*       array of valid request and returns which number they are, it returns -1 if it isn't
*       a valid code
*/
int determine_request(char * request);

#endif
