#include "client_request_handler.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "safe_socket.h"


void authentication_procedure() {
    char buffer[BUFFER_SIZE];
    while (true) {
        printf("Enter the server password:\n");
        scanf("%s", buffer);
        while (getchar() != '\n') {};
        if (strcmp(buffer, SERVER_PASSWORD) == 0) {
            break;
        }
        printf("Incorrect password. Please try again.\n");
    }
}

void modify_procedure() {
    char buffer[BUFFER_SIZE];
    printf("Enter the desired operation:\n"
           "1. Add a new contact\n"
           "2. Remove an contact\n");
    scanf("%s", buffer);
    while (getchar() != '\n') {};
    write_message(client_socket, buffer);
    read_message(client_socket, buffer);
    printf("Received: %s\n", buffer);
}