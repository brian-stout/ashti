#include "response.h"

char OK_200[] =             "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html\r\n\r\n"
                            "\r\n";

char OK_200_cgi[] =         "HTTP/1.1 200 OK\r\n";

char not_found_404[] =      "HTTP/1.1 404 Not Found\r\n"
                            "Content-Type: text/html \r\n\r\n"
                            "\r\n";

char not_found_text[] =     "<center><h1>404 Not found</h1></center> \r\n\r\n";

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
    memset(&buf, '\0', sizeof(buf));
    char * response = NULL;
  
    response = OK_200;
    send(remote, response, strlen(response), 0);
    while(fgets(buf, sizeof(buf), fp) != NULL) {
        send(remote, buf, strlen(buf), 0);
    }

    return 1;
}

int cgi_response(char * token, int remote)
{
    //TODO: Cgi_error respons is 404

    char buf[256];
    memset(&buf, '\0', sizeof(buf));
    char * response = NULL;

    FILE * process = NULL;
    printf("CGI DEBUG: token: %s\n", token);

    process = popen(token+1, "r");

    if(process) {
        printf("Successfully opened process!\n");
        response = OK_200_cgi;
        send(remote, response, strlen(response), 0);
        while(fgets(buf, sizeof(buf), process) != NULL) {
            send(remote, buf, strlen(buf), 0);
        }
        pclose(process);
        if (buf[0] == '\0') {
            return -1;
        }
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
