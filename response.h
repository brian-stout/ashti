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

/** get_response() is responsible for sending all the data back to the client for a regular
*       get request
*/
int get_response(FILE * fp, int remote);

/** cgi_response() is responsible for sending back the data to the client for a cgi-bin request.
*       It opens up a process with popen to do this, so the script must be executable.
*/
int cgi_response(char * token, int remote);

/** error_***() functions print out premade responses to specific errors, or check specifically
*       for a ***.html in the error folder and send that back to the client.  There's some overlap
*       here, but for the sake of time (it would have been more complicated creating a robust
*        parameter based function) it's done seperately.
*/
void error_404(int remote);
void error_400(int remote);
void error_500(int remote);

#endif
