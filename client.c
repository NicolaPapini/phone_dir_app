#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "client_request_handler.h"
#include "safe_socket.h"
#include "server_response_parser.h"
#include "data_structures/cJSON.h"

int main(int arc, char *argv[]) {
    if (arc != 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }
    int operation = 0;
    bool authenticated = false;
    int client_socket = create_client_socket(argv[1]);
    char *end_ptr;
    char buffer[BUFFER_SIZE];

    printf("Welcome to the phone directory application!\n\n");

    do {
        printf("\n==============================================================================================\n");
        printf("Enter the number corresponding to the desired operation:\n\n"
               "1. Add a new contact\n"
               "2. Remove a contact\n"
               "3. Update a contact number\n"
               "4. Search for contacts by full name prefix\n"
               "5. Search for a contact by number\n"
               "6. Exit\n");
        do {
            fgets(buffer, sizeof(buffer), stdin);
            operation = strtol(buffer, &end_ptr, 10);

            if (*end_ptr != '\0' && *end_ptr != '\n') {
                printf("Invalid input. Please enter a valid number.\n");
            }

        } while (*end_ptr != '\0' && *end_ptr != '\n' || operation < 1 || operation > 6);

        if (operation == 6) {
            break;
        }

        if (operation <= 3 && !authenticated) {
            authentication_procedure();
            authenticated = true;
        }
        handle_request(operation, client_socket);
        char *response = calloc(BUFFER_SIZE, sizeof(char));
        read_message(client_socket, response);
        cJSON *response_json = cJSON_Parse(response);
        parse_response(response_json);
    } while (true);

    printf("Closing connection\n");
    close(client_socket);
    return EXIT_SUCCESS;
}
