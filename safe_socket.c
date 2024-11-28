#include "safe_socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common_utils.h"
#define MESSAGE_TERMINATOR '}'

ssize_t write_message(int socket, char *buffer) {
    ssize_t total_size = strlen(buffer);
    ssize_t msg_size = 0;
    ssize_t bytes_written;

    // Send the message length first (4 bytes)
    uint32_t message_length = htonl(total_size); // Convert to network byte order
    if (write(socket, &message_length, sizeof(message_length)) != sizeof(message_length)) {
        perror("write length");
        return -1;
    }

    // Send the actual message
    while (msg_size < total_size) {
        bytes_written = write(socket, buffer + msg_size, total_size - msg_size);
        if (bytes_written < 0) {
            perror("write");
            return -1;
        }
        msg_size += bytes_written;
    }
    return msg_size;
}

ssize_t read_message(int socket, char *buffer) {
    ssize_t msg_size = 0;
    ssize_t bytes_read;

    // Read the 4-byte length prefix
    uint32_t message_length;
    if (read(socket, &message_length, sizeof(message_length)) != sizeof(message_length)) {
        perror("read length");
        return -1;
    }
    message_length = ntohl(message_length); // Convert from network byte order

    if (message_length > BUFFER_SIZE - 1) {
        fprintf(stderr, "Message length exceeds buffer size\n");
        return -1;
    }

    // Read the actual message
    while (msg_size < message_length) {
        bytes_read = read(socket, buffer + msg_size, message_length - msg_size);
        if (bytes_read < 0) {
            perror("read");
            return -1;
        } else if (bytes_read == 0) {
            fprintf(stderr, "Connection closed unexpectedly\n");
            return -1;
        }
        msg_size += bytes_read;
    }

    buffer[msg_size] = '\0'; // Null-terminate the string

    return msg_size;
}


struct sockaddr_in create_addr_struct(const char *ip_address) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);

    if(ip_address == NULL) {
        addr.sin_addr.s_addr = INADDR_ANY;
    } else {
        int result = inet_pton(AF_INET, ip_address, &addr.sin_addr);
        if (result <= 0) {
            printf("Invalid address\n");
            exit(EXIT_FAILURE);
        }
    }
    return addr;
}

int create_server_socket() {
    int socket_fd;
    struct sockaddr_in server_address = create_addr_struct(NULL);
    check (socket_fd = socket(AF_INET, SOCK_STREAM, 0),
        "Socket creation failed");
    check (bind(socket_fd, (struct sockaddr*) &server_address, sizeof(server_address)),
        "Binding failed");
    check (listen(socket_fd, BACKLOG),
        "Listening failed");
    return socket_fd;
}

int create_client_socket(const char *ip_address) {
    int socket_fd;
    struct sockaddr_in server_address = create_addr_struct(ip_address);
    check (socket_fd = socket(AF_INET, SOCK_STREAM, 0),
        "Socket creation failed");
    check (connect(socket_fd, (struct sockaddr*) &server_address, sizeof(server_address)),
        "Connection failed");
    return socket_fd;
}

int accept_connection(int server_socket) {
    int client_socket;
    struct sockaddr_in client_address;
    int addr_size = sizeof(struct sockaddr_in);
    check (client_socket = accept(server_socket, (struct sockaddr*) &client_address, (socklen_t*) &addr_size),
        "Accepting client failed");
    return client_socket;
}

