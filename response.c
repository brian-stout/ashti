#include "response.h"

char OK_200[] =             "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html\r\n\r\n"
                            "\r\n";

char OK_200_cgi[] =         "HTTP/1.1 200 OK\r\n";

char not_found_404[] =      "HTTP/1.1 404 Not Found\r\n"
                            "Content-Type: text/html \r\n\r\n"
                            "\r\n";

char not_found_text[] =     "<center><h1>404 Not found</h1></center>\r\n\r\n";

char server_error_500[] =   "HTTP/1.1 500 Internal Server Error\r\n"
                            "Content-Type: text/html \r\n\r\n"
                            "\r\n";

char server_error_text[] =  "<center><h1>500 Server Error</h1></center>\r\n\r\n";

char bad_request_400[] =   "HTTP/1.1 400 Bad Request \r\n"
                            "Content-Type: text/html \r\n\r\n"
                            "\r\n";

char text_request_text[] =  "<center><h1>400 Bad request</h1></center>\r\n\r\n";

int get_response(FILE * fp, int remote)
{
    char buf[512];
    //Init buf to avoid valgrind error
    memset(&buf, '\0', sizeof(buf));
    char * response = NULL;
  
    response = OK_200;
    //Send the basic response
    send(remote, response, strlen(response), 0);
    //Send the webpage back in chunks
    while(fgets(buf, sizeof(buf), fp) != NULL) {
        send(remote, buf, strlen(buf), 0);
    }

    return 1;
}

int cgi_response(char * token, int remote)
{
    char buf[512];
    //Init buf to avoid valgrind error
    memset(&buf, '\0', sizeof(buf));
    char * response = NULL;

    FILE * process = NULL;

    //Makes sure a file exists before attempting to popen it
    //  This is not a good solution to check to see if popen actually opened
    //  a process, since it might not be executable.  But unfortunately
    //  when popen can't open a process it causes a shell error
    //  and I have no idea how to capture that
    if(check_file_exists(token+1) == false) {
        printf("Missing CGI script\n");
        return -1;
    }

    //Avoid the first / so the program knows it's a relative path
    process = popen(token+1, "r");

    if(process) {
        //Send initial response
        response = OK_200_cgi;
        send(remote, response, strlen(response), 0);
        //Send the resulting output from popen's stdout to the client
        while(fgets(buf, sizeof(buf), process) != NULL) {
            send(remote, buf, strlen(buf), 0);
        }
        pclose(process);

        //Checks to see if anything ever existed in buf.  Kind of a hackey way to determine
        // if popen actually worked, but I can't find another solution since
        // poopen doesn't actually error when it can't find the process, the shell does
        if (buf[0] == '\0') {
            return -1;
        }
    //Shouldn't happen
    } else {
        printf("POPEN ERROR: Fork or Malloc Failed\n");
        return 1;
    }

    return 0;
}

void error_404(int remote)
{
    char * response = NULL;
    FILE * fp = NULL;
    char buf[256];
    //Use this to initialize buf and prevent a valgrind error
    memset(&buf, '\0', sizeof(buf));

    response = not_found_404;
    send(remote, response, strlen(response), 0);
    //Checks to see if a premade 404 html page
    if(check_file_exists("error/404.html") == true) {
        fp = fopen("error/404.html", "r");
        fread(&buf, sizeof(buf), sizeof(char), fp);
        fclose(fp);
        send(remote, buf, strlen(buf), 0);
    } else {
        response = not_found_text;
        send(remote, response, strlen(response), 0);
   }
}

void error_500(int remote)
{
    char * response = NULL;
    FILE * fp = NULL;
    char buf[256];
    //Use this to initialize buf and prevent a valgrind error
    memset(&buf, '\0', sizeof(buf));

    response = server_error_500;
    send(remote, response, strlen(response), 0);
    if(check_file_exists("error/500.html") == true) {
        fp = fopen("error/500.html", "r");
        fread(&buf, sizeof(buf), sizeof(char), fp);
        fclose(fp);
        send(remote, buf, strlen(buf), 0);
    } else {
        response = server_error_text;
        send(remote, response, strlen(response), 0);
   }
}

void error_400(int remote)
{
    char * response = NULL;
    FILE * fp = NULL;
    char buf[256];
    //Use this to initialize buf and prevent a valgrind error
    memset(&buf, '\0', sizeof(buf));

    response = bad_request_400;
    send(remote, response, strlen(response), 0);
    if(check_file_exists("error/400.html") == true) {
        fp = fopen("error/400.html", "r");
        fread(&buf, sizeof(buf), sizeof(char), fp);
        fclose(fp);
        send(remote, buf, strlen(buf), 0);
    } else {
        response = text_request_text;
        send(remote, response, strlen(response), 0);
   }
}
