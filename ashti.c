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
 
char response[] = "HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html><html><head><title>Bye-bye baby bye-bye</title>"
"<style>body { background-color: #111 }"
"h1 { font-size:4cm; text-align: center; color: black;"
" text-shadow: 0 0 2mm red}</style></head>"
"<body><h1>Goodbye, world!</h1></body></html>\r\n";

// The next 39 lines come from the day02/udp_server
// The next 39 lines come from the day02/udp_server
int run_server(int remote)
{
    char buf[512];


    read(remote, buf, sizeof(buf));
    send(remote, response, strlen(response), 0);
    send(remote, buf, strlen(buf), 0);

    close(remote);
    return 0;
}

int main(int argc, char *argv[])
{
    if(argc != 2) {
        fprintf(stderr, "%s <IP>\n", argv[0]);
        return 1;
    }

    // Port numbers are in the range 1-65535, plus null byte
    char port_num[8];
    snprintf(port_num, sizeof(port_num), "%hu", getuid());

    struct addrinfo *results;
    struct addrinfo hints = {0};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    int err = getaddrinfo(argv[1], port_num, &hints, &results);
    if(err != 0) {
        fprintf(stderr, "Could not parse address: %s\n", gai_strerror(err));
        return 2;
    }

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
