#include "safe_socket.h"
#define MESSAGE_TERMINATOR '\n'

//TODO Modify the read_message and write_message functions to fit with the JSON format
ssize_t write_message(int socket, char *buffer) {
    size_t len = strlen(buffer);
    char *terminated_buffer = malloc(len + 2);  // +2 for newline and null terminator
    strcpy(terminated_buffer, buffer);
    terminated_buffer[len] = MESSAGE_TERMINATOR;
    terminated_buffer[len + 1] = '\0';

    ssize_t total_size = len + 1;  // Include the terminator
    ssize_t msg_size = 0;
    ssize_t bytes_written;

    while (msg_size < total_size) {
        bytes_written = write(socket, terminated_buffer + msg_size, total_size - msg_size);
        if (bytes_written < 0) {
            perror("write");
            free(terminated_buffer);
            return -1;
        }
        msg_size += bytes_written;
    }

    free(terminated_buffer);
    return msg_size;
}

ssize_t read_message(int socket, char *buffer) {
    ssize_t msg_size = 0;
    ssize_t bytes_read;

    while (msg_size < BUFFER_SIZE - 1) {
        bytes_read = read(socket, buffer + msg_size, 1);  // Read one byte at a time

        if (bytes_read < 0) {
            perror("read");
            return -1;
        }

        if (bytes_read == 0) {  // Connection closed by peer
            return 0;
        }

        if (buffer[msg_size] == MESSAGE_TERMINATOR) {
            buffer[msg_size] = '\0';  // Replace terminator with null
            return msg_size;
        }

        msg_size += bytes_read;
    }

    buffer[msg_size] = '\0';  // Ensure null termination
    return msg_size;
}

void check(int value, char *msg) {
    if (value == SOCKET_ERROR) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
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

