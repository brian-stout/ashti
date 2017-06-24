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
#include "files.h"
#include "request.h"
#include "response.h"

const char * www_root_missing =     "ERROR : www_root does not exist in the"
                                    " folder where the binary is located,"
                                    " or in the directory specified from a"
                                    " command line argument.  Either move\n"
                                    " the binary to a folder containing www_root"
                                    " or specify a path to a custom root directory.\n"
                                    "USAGE: ./ashti *<path to www_root>\n";

const char * directory_is_missing = "ERROR : the specified directory does"
                                    " not exist.\n"
                                    "USAGE: ./ashti <path to www_root>\n";

int run_server(int remote);

// The next 39 lines come from the day02/udp_server
// The next 39 lines come from the day02/udp_server
int main(int argc, char *argv[])
{
    errno = 0;
    char directory[256];

    if(argc == 1) {
        if (check_dir_exists("www_root") == false) {
            fprintf(stderr, "%s\n", www_root_missing);
            return EX_USAGE;
        }
        chdir("www_root");
    } else if(argc == 2) {
        if (check_dir_exists(argv[1]) == false) {
            fprintf(stderr, "%s\n", directory_is_missing);
            return EX_USAGE;
        } else {
            chdir(argv[1]);
        }
    } else {
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

    const char * host = "localhost";

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
    char read_buf[512];
    //TODO: FSEEK FP stream to determine size and malloc memory
    //      It's fine with a normal buffer for now, just because only do two things
    //      with the request header
    char write_buf[256];
    char * response = NULL;
    memset(&write_buf, '\0', sizeof(write_buf));
    read(remote, read_buf, sizeof(read_buf));

    char *token = NULL;

    token = strtok(read_buf, " ");
    int switch_int = determine_request(token);
    FILE * fp = NULL;
    bool cgi_bin_request = false;

    int err = 0;

    switch(switch_int) {
        case 1:
            token = strtok(NULL, " ");
            if(detect_path_traversal(token) == false) {
                error_400(remote);
                break;
            }
            fp = get_request(token, &cgi_bin_request);
            if(cgi_bin_request == true) {
                err = cgi_response(token, remote);
                if (err < 0) {
                    error_404(remote);
                }
                break;
            }
            if(fp) {
                err = get_response(fp, remote);
                fclose(fp);
            } else {
                error_404(remote);
            }
            break;
        default:
            error_500(remote);
    }

    close(remote);
    return 0;
}



