#ifndef SAFE_SOCKET_H
#define SAFE_SOCKET_H

#include <sys/socket.h>

#define BACKLOG 10
#define PORT 8080
#define BUFFER_SIZE 4096
#define SERVER_PASSWORD "qwerty"
#define NAME_SIZE 60
#define SURNAME_SIZE 60
#define NUMBER_SIZE 60

typedef enum {
    ADD_CONTACT,
    DELETE_CONTACT,
    UPDATE_CONTACT,
    SEARCH_BY_PREFIX,
    SEARCH_BY_NUMBER,
} Operation;


ssize_t write_message(int socket, char *buffer);
ssize_t read_message(int socket, char *buffer);
int create_server_socket();
int create_client_socket(const char *ip_address);
int accept_connection(int server_socket);

#endif //SAFE_SOCKET_H
