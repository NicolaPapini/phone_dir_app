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


ssize_t write_message(int socket, char *buffer);
ssize_t read_message(int socket, char *buffer);
void check(int value, char *msg);
int create_server_socket();
int create_client_socket(const char *ip_address);
int accept_connection(int server_socket);

#endif //SAFE_SOCKET_H
