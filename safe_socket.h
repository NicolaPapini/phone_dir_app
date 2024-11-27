#ifndef SAFE_SOCKET_H
#define SAFE_SOCKET_H

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <secure/_string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BACKLOG 10
#define PORT 8080
#define SOCKET_ERROR -1
#define BUFFER_SIZE 1024
#define SERVER_PASSWORD "qwerty"
#define NAME_SIZE 50
#define SURNAME_SIZE 50
#define NUMBER_SIZE 32

typedef enum {
    ADD_CONTACT,
    DELETE_CONTACT,
    UPDATE_CONTACT,
    SEARCH_BY_PREFIX,
    SEARCH_BY_NUMBER,
} Operation;


ssize_t write_message(int socket, char *buffer);
ssize_t read_message(int socket, char *buffer);
void check(int value, char *msg);
int create_server_socket();
int create_client_socket(const char *ip_address);
int accept_connection(int server_socket);

#endif //SAFE_SOCKET_H
