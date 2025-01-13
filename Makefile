CC = gcc
CFLAGS = -Wall -std=gnu11
SERVER_SOURCES = server.c safe_socket.c server_connection_handler.c \
                 common_utils.c connection_utils.c serialization.c \
                 data_structures/queue.c data_structures/cJSON.c \
                 data_structures/phone_directory.c data_structures/hash_map.c

CLIENT_SOURCES = client.c safe_socket.c client_request_handler.c \
                 common_utils.c connection_utils.c \
                 server_response_parser.c data_structures/cJSON.c

all: server client


server: $(SERVER_SOURCES)
	$(CC) $(CFLAGS) -o server $(SERVER_SOURCES)

client: $(CLIENT_SOURCES)
	$(CC) $(CFLAGS) -o client $(CLIENT_SOURCES)

clean:
	rm -f server client
	rm -f phone_directory.txt

.PHONY: all clean