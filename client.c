#include <stdbool.h>

#include "safe_socket.h"

int main(int arc, char *argv[]) {
    if (arc != 2) {
        printf("Usage: %s <server_ip>\n", argv[0]);
        return 1;
    }
    int count = 0;

    int client_socket = create_client_socket(argv[1]);
    char buffer[BUFFER_SIZE];

    printf("Welcome to the phone directory application!\n");

    do {
        printf("Enter a string to send to the server (\"Exit\" to quit): ");
        scanf("%s", buffer);
        while (getchar() != '\n') {};

        if (strcmp(buffer, "Exit") == 0) {
            break;
        }

        write_message(client_socket, buffer);
        read_message(client_socket, buffer);
        printf("Received: %s\n", buffer);
        count++;
    } while (true);

    printf("Closing connection\n");
    close(client_socket);
    printf("iteration count: %d\n", count);
    return EXIT_SUCCESS;
}