/**
 * Client implementation
 * 
 * @author Jerome Dh
 * @date 05/09/2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "client.h"

/**
 * Start the client
 */
static void init(void) {

    #ifdef WIN32
        WSADATA wsa;
        int err = WSAStartup(MAKEWORD(2, 2), &wsa);
        if(err < 0) {
            puts("WSAStartup failed !");
            exit(EXIT_FAILURE);
        }
    #endif
}

/**
 * Stop the client
 */
static void end(void) {

    #ifdef WIN32
        WSACleanup();
    #endif
}

/**
 * Run the main app
 */ 
static void app(const char *address, const char *name) {

    SOCKET sock = init_connection(address);
    char buffer[BUF_SIZE];

    fd_set rdfs;

    // Send our name to server
    write_server(sock, name);

    while(1) {

        // Erase all descriptors on set
        FD_ZERO(&rdfs);

        // Add STDIN_FILENO on set of descriptors
        FD_SET(STDIN_FILENO, &rdfs);

        // Add the socket
        FD_SET(sock, &rdfs);

        if(select(sock + 1, &rdfs, NULL, NULL, NULL) == -1) {
            perror("select()");
            exit(errno);
        }

        // Something form standard input: i.e keyborad
        if(FD_ISSET(STDIN_FILENO, &rdfs)) {
            fgets(buffer, BUF_SIZE -1, stdin);
            {
                char *p = NULL;
                p = strstr(buffer, "\n");
                if(p != NULL) {
                    *p = 0;
                }
                else {
                    // fclean
                    buffer[BUF_SIZE -1] = 0;
                }
            }

            write_server(sock, buffer);

        }
        else if(FD_ISSET(sock, &rdfs)) {

            int n = read_server(sock, buffer);

            // Server down
            if(n == 0) {
                printf("Server disconnect\n");
                break;
            }

            puts(buffer);
        }

    }

    // Close the connection
    end_connection(sock);
}

/**
 * Init the connection
 */ 
static int init_connection(const char *address) {

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN sin = { 0 };
    struct hostent *hostinfo;

    if(sock == INVALID_SOCKET) {
        perror("socket()");
        exit(errno);
    }

    hostinfo = gethostbyname(address);
    if(hostinfo == NULL) {
        fprintf(stderr, "Unknown host %s. \n", address);
        exit(EXIT_FAILURE);
    }

    sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr;
    sin.sin_port = htons(PORT);
    sin.sin_family = AF_INET;

    if(connect(sock, (SOCKADDR *) &sin, sizeof(SOCKADDR)) == SOCKET_ERROR) {
        perror("connect()");
        exit(errno);
    }

    return sock;
}

/**
 * Close the connection
 */
static void end_connection(int sock) {
    closesocket(sock);
}

/**
 * Read the message send by the server
 */ 
static int read_server(SOCKET sock, char *buffer) {
    
    int n = 0;

    if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0) {
        perror("recv()");
        exit(errno);
    }
    buffer[n] = 0;

    return n;
}

/**
 * Write a message to send to server
 */
static void write_server(SOCKET sock, const char *buffer) {

    if(send(sock, buffer, strlen(buffer), 0) < 0) {
        perror("send()");
        exit(errno);
    }
}

/**
 * Main function
 */ 
int main(int argc, char **argv) {

    if(argc < 2) {
        printf("Usage : %s [address] [pseudo]\n", argv[0]);
        return EXIT_FAILURE;
    }

    init();

    app(argv[1], argv[2]);

    end();

    return EXIT_SUCCESS;
}