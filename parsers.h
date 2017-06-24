#ifndef PARSERS_H
#define PARSERS_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

FILE * GET_response(char * request, bool * cgi_bin_request);
int determine_request(char * request);

#endif
