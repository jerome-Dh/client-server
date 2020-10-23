
/**
 * Client header
 * 
 * @author Jerome Dh
 * @date 05/09/2020
 */

#ifndef CLIENT_H
#define CLIENT_H
#ifdef WIN32

// In Windows platform
#include <winsock2.h>

#elif defined (linux)

// In linux platform
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> // Close
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

#endif

#define CRLF        "\r\n"
#define PORT        1977
#define BUF_SIZE    1024

static void init(void);
static void end(void);
static void app(const char *address, const char *name);
static int init_connection(const char *address);
static void end_connection(int sock);
static int read_server(SOCKET sock, char *buffer);
static void write_server(SOCKET sock, const char *buffer);

#endif // CLIENT_H
