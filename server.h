/**
 * Server header
 * 
 * @author Jerome Dh
 * @date 05/09/2020
 */
#ifndef SERVER_H
#define SERVER_H

#ifdef WIN32

#include <winsock2.h>

#elif defined (linux)

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <unistd.h> // close
#include <netdb.h> // gethostbyname

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket(s) close(s)

typedef int SOCKET;
typedef struct sockaddr_in SOCKADDR_IN;
typedef struct sockaddr SOCKADDR;
typedef struct in_addr IN_ADDR;

#else

#error Not defined for this platform

#endif // (linux)

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define CRLF        "\r\n"
#define PORT        1977
#define MAX_CLIENTS 100
#define BUF_SIZE    1024

typedef struct 
{
    SOCKET sock;
    char name[BUF_SIZE];
} Client;

void init(void);
void end(void);
void app(void);
static int init_connection(void);
static void end_connection(int sock);
static int read_client(SOCKET sock, char *buffer);
static void write_client(SOCKET sock, const char *buffer);
static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server);
static void remove_client(Client *clients, int to_remove, int *actual);
static void clear_clients(Client *clients, int actual);

#endif // SERVER_H
