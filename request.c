#include "request.h"

const char * header_code[] = { "OPTIONS", "GET", "HEAD", "POST",
                                "PUT", "DELETE", "TRACE", "CONNECT", NULL};

int determine_request(char * request)
{
    for(int i = 0; header_code[i] != NULL; i++)
    {
        int cmp = 0;
        //Checks to see if string is the same and returns that index if it is
        cmp = strncmp(header_code[i], request, strlen(request));
        if (cmp == 0) {
            return i;
        } 
    }
    return -1;
}

FILE * get_request(char * request, bool * cgi_bin_request)
{
    FILE * fp = NULL;
    char buf[256];
    //Memset to avoid an init error from valgrind
    memset(&buf, '\0', sizeof(buf));

    //If it's just a / than open up index.html by default
    if(request[0] == '/' && request[1] == '\0') {
        fp = fopen("www/index.html", "r");
        if(fp) {
            return fp;
        }
        else {
            return NULL;
        }
    } else {
        //If the first 7 letters are cgi-bin send the request to the cgi_response function
        if(strncmp("cgi-bin", request+1, 7) == 0){
            *cgi_bin_request = true;
            return NULL;
        } else {
            //If everthing else make the path relative from the www folder and 
            //  open a file pointer to that path
            snprintf(buf, sizeof(buf), "www%s", request);
            fp = fopen(buf, "r");
        }
        return fp;
    }
}
