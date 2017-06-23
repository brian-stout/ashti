// This whole file comes from day03/unitcp.c
#define _GNU_SOURCE

#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/types.h>
#include "directories.h"
 
char response[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"\r\n";

// The next 39 lines come from the day02/udp_server
// The next 39 lines come from the day02/udp_server
int run_server(int remote);

/**** TODO LIST:
 *  1.  Find directory path of WWW_ROOT (Make sure relative paths work too)
 *  2.  Parse command given from html browser (error handle if bad)
 *  3.  Look for file called for or default to index html (404 if can't find)
 *  4.  Make the CGI-BIN work
*****/

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

    // Port numbers are in the range 1-65535, plus null byte
    char port_num[8];
    snprintf(port_num, sizeof(port_num), "%hu", getuid());


    struct addrinfo *results;
    struct addrinfo hints = {0};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    const char * host = "tooldev";

    int err = getaddrinfo(host, port_num, &hints, &results);
    if(err != 0) {
        fprintf(stderr, "Could not parse address: %s\n", gai_strerror(err));
        return 2;
    }

    printf("Server address %s:%hu\n", host, getuid());

    int sd = socket(results->ai_family, results->ai_socktype, 0);
    if(sd < 0) {
        perror("Could not create socket");
        freeaddrinfo(results);
        return 3;
    }

    int set = 1;
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set));

    err = bind(sd, results->ai_addr, results->ai_addrlen);
    if(err < 0) {
        perror("Could not bind");
        close(sd);
        freeaddrinfo(results);
        return 4;
    }
    freeaddrinfo(results);

    // 5 is the usual backlog
    err = listen(sd, 5);
    if(err < 0) {
        perror("Could not listen");
        freeaddrinfo(results);
        return 4;
    }

    struct sigaction ignorer = {0};
    ignorer.sa_handler = SIG_IGN;
    sigaction(SIGCHLD, &ignorer, NULL);

    // Next 36 lines also pulled from say02/udp_server
    for(;;) {
        struct sockaddr_storage client;
        socklen_t client_sz = sizeof(client);
        char ip[INET6_ADDRSTRLEN];
        unsigned short port;

        int remote = accept(sd, (struct sockaddr *)&client, &client_sz);
        if(remote < 0) {
            perror("Could not accept connection");
            continue;
        }

        pid_t child = fork();

        // The child process handles the request
        if(child == 0) {

            if(client.ss_family == AF_INET6) {
                inet_ntop(client.ss_family,
                        &((struct sockaddr_in6 *)&client)->sin6_addr,
                        ip, sizeof(ip));
                port = ntohs(((struct sockaddr_in6 *)&client)->sin6_port);
            } else {
                inet_ntop(client.ss_family,
                        &((struct sockaddr_in *)&client)->sin_addr,
                        ip, sizeof(ip));
                port = ntohs(((struct sockaddr_in *)&client)->sin_port);
            }

            close(sd);
            return run_server(remote);
        }
        else if(child < 0) {
            perror("Could not spawn worker");
            continue;
        }

        close(remote);
    }
}

int run_server(int remote)
{
    char buf[512];


    read(remote, buf, sizeof(buf));
    send(remote, response, strlen(response), 0);
    send(remote, buf, strlen(buf), 0);

    close(remote);
    return 0;
}

