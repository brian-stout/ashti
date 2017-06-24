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

FILE * get_request(char * request, bool * cgi_bin_request);
int determine_request(char * request);

#endif
