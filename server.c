/**
 * Lib core server
 * 
 * @author Jerome 
 * @date 06/09/2020
 */
#include "server.h"

/**
 * Init the server
 */ 
void init(void) {
    
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
 * End up to server
 */ 
void end(void) {
    #ifdef WIN32
        WSACleanup();
    #endif
}

/**
 * Run app
 */ 
void app(void) {

    SOCKET sock = init_connection();
    char buffer[BUF_SIZE];

    // The index of the array
    int actual = 0;
    int max = sock;

    // Array for all clients
    Client clients[MAX_CLIENTS];

    fd_set rdfs;

    printf("I listen on port %d.\n", PORT);

    while(1) {

        int i = 0;
        FD_ZERO(&rdfs);

        // Add STDIN_FILENO
        FD_SET(STDIN_FILENO, &rdfs);

        // Add the connection socket
        FD_SET(sock, &rdfs);

        // Add socket of each client
        for(i = 0; i < actual; i++) {
            FD_SET(clients[i].sock, &rdfs);
        }

        if(select(max + 1, &rdfs, NULL, NULL, NULL) == -1) {

            perror("select()");
            exit(errno);
        }

        // Something from standard input : i.e keyboard
        if(FD_ISSET(STDIN_FILENO, &rdfs)) {

            // Stop process when type on keyboard
            printf("A key typed, bye !\n");
            break;
        }
        else if(FD_ISSET(sock, &rdfs)) {

            // New client
            SOCKADDR_IN csin = { 0 };
            unsigned int sinsize = sizeof csin;
            int csock = accept(sock, (SOCKADDR *) &csin, &sinsize);

            if(csock == SOCKET_ERROR) {
                perror("accept()");
                continue;
            }

            // After connecting the client sends its name
            if(read_client(csock, buffer) == -1) {

                // Disconnected
                continue;
            }

            // What is the new maximum fd ?
            max = csock > max ? csock : max;

            FD_SET(csock, &rdfs);

            Client c = { csock };
            strncpy(c.name, buffer, BUF_SIZE - 1);
            clients[actual] = c;

            // Send successful connection to client
            char message[BUF_SIZE];
            strncpy(message, "Server : Great, you are successful login !", BUF_SIZE - 1);
            write_client(clients[actual].sock, message);

            printf("- %s connected\n", clients[actual].name);

            actual++;
        }
        else {

            int i = 0;

            for(i = 0; i < actual; i++) {

                // A client is talking
                if(FD_ISSET(clients[i].sock, &rdfs)) {

                    Client client = clients[i];
                    int c = read_client(clients[i].sock, buffer);

                    // Client disconnected
                    if(c == 0) {

                        closesocket(clients[i].sock);
                        remove_client(clients, i, &actual);
                        strncpy(buffer, client.name, BUF_SIZE - 1);
                        strncat(buffer, " disconnected !", BUF_SIZE - strlen(buffer) - 1);
                        send_message_to_all_clients(clients, client, actual, buffer, 1);
                        printf("- %s disconnected.\n", client.name);
                    }
                    else {
                        send_message_to_all_clients(clients, client, actual, buffer, 0);
                    }

                    break;

                }
            }
        }

    }

    clear_clients(clients, actual);
    end_connection(sock);
}


/**
 * Close connection to clients
 */ 
static void clear_clients(Client *clients, int actual) {

    int i = 0;
    for(i = 0; i < actual; i++) {
        closesocket(clients[i].sock);
    }
}

/**
 * Remove a client from a list
 */ 
static void remove_client(Client *clients, int to_remove, int *actual) {

    // Remove the client in the array
    memmove(clients + to_remove, clients + to_remove + 1, (*actual - to_remove -1) * sizeof(clients));

    // Number client - 1
    (*actual)--;
}

/**
 * Send a message to all clients
 */ 
static void send_message_to_all_clients(Client *clients, Client sender, int actual, const char *buffer, char from_server) {

    int i = 0;
    char message[BUF_SIZE];
    message[0] = 0;

    for(i = 0; i < actual; i++) {

        // Not send message to sender
        if(sender.sock != clients[i].sock) {

            if(from_server == 0) {
                strncpy(message, sender.name, BUF_SIZE - 1);
                strncat(message, " : ", sizeof message - strlen(message) - 1);
            }

            strncat(message, buffer, sizeof message - strlen(message) - 1);

            printf("Message <<%s>> sended to <<%s>>.\n", message, clients[i].name);
            write_client(clients[i].sock, message);

        }
    }
}

/**
 * Init the connection
 */ 
static int init_connection(void) {

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN sin = { 0 };

    if(sock == INVALID_SOCKET) {

        perror("socket()");
        exit(errno);
    }

    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(PORT);
    sin.sin_family = AF_INET;

    if(bind(sock, (SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR) {

        perror("bind()");
        exit(errno);
    }

    if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR) {

        perror("bind()");
        exit(errno);
    }

    return sock;

}

/**
 * Close all connections
 */ 
static void end_connection(int sock) {

    closesocket(sock);
}

/**
 * Receive a message from a client
 */ 
static int read_client(SOCKET sock, char *buffer) {

    int n = 0;
    if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0) {
        perror("recv()");
        n = 0; // Disconnect the client
    }

    buffer[n] = 0;

    return n;

}

/**
 * Send the message to a client
 */ 
static void write_client(SOCKET sock, const char *buffer) {

    if(send(sock, buffer, strlen(buffer), 0) < 0) {

        perror("send()");
        exit(errno);
    }

}
