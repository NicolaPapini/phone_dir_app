#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "connection_utils.h"
#include "safe_socket.h"
#include "client_request_handler.h"

void client_add(char *buffer);
void client_delete(char *buffer);
void client_update(char *buffer);
void client_search_by_prefix(char *buffer);
void client_search_by_number(char *buffer);

void authentication_procedure() {
    printf("Authentication required to modify the directory. Enter the server password:\n");
    while (true) {
        char buffer[BUFFER_SIZE];
        scanf("%s", buffer);
        while (getchar() != '\n') {};
        if (strcmp(buffer, SERVER_PASSWORD) == 0) {
            break;
        }
        printf("Incorrect password. Please try again.\n");
    }
    printf("Authentication successful. You can now modify the directory.\n\n");
}

void handle_request(int operation, int client_socket) {
    char *buffer = calloc(BUFFER_SIZE, sizeof(char));
    switch (operation-1) {
        case ADD_CONTACT:
            client_add(buffer);
            break;
        case DELETE_CONTACT:
            client_delete(buffer);
            break;
        case UPDATE_CONTACT:
            client_update(buffer);
            break;
        case SEARCH_BY_PREFIX:
            client_search_by_prefix(buffer);
            break;
        case SEARCH_BY_NUMBER:
            client_search_by_number(buffer);
            break;
        default:
            printf("Invalid operation\n");
            exit(EXIT_FAILURE);
    }
    write_message(client_socket, buffer);
    free(buffer);
}

void client_add(char *buffer) {
    char name[NAME_SIZE], surname[SURNAME_SIZE], number[NUMBER_SIZE];
    prompt_fields(name, surname, number, NULL);
    build_JSON_request(buffer, "ADD_CONTACT", name, surname, number, NULL);
}

void client_delete(char *buffer) {
    char name[NAME_SIZE], surname[SURNAME_SIZE], number[NUMBER_SIZE];
    prompt_fields(name, surname, number, NULL);
    build_JSON_request(buffer, "DELETE_CONTACT", name, surname, number, NULL);
}

void client_update(char *buffer) {
    char number[NUMBER_SIZE], new_number[NUMBER_SIZE];
    prompt_fields(NULL, NULL, number, new_number);
    build_JSON_request(buffer, "UPDATE_CONTACT", NULL, NULL, number, new_number);
}

void client_search_by_prefix(char *buffer) {
    char name[NAME_SIZE], surname[SURNAME_SIZE];
    prompt_fields(name, surname, NULL, NULL);
    build_JSON_request(buffer, "SEARCH_BY_PREFIX", name, surname, NULL, NULL);
}

void client_search_by_number(char *buffer) {
    char number[NUMBER_SIZE];
    prompt_fields(NULL, NULL, number, NULL);
    build_JSON_request(buffer, "SEARCH_BY_NUMBER", NULL, NULL, number, NULL);
}
