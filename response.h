#ifndef RESPONSE_H
#define RESPONSE_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "files.h"

int get_response(FILE * fp, int remote);
int cgi_response(char * token, int remote);
void error_404(int remote);
void error_400(int remote);
void error_500(int remote);

#endif
