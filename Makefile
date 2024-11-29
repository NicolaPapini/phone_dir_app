# Compiler and flags
CC = gcc
CFLAGS = -g -Wall -std=gnu11

# Directories
SRC_DIR = .
DATA_STRUCTURES_DIR = $(SRC_DIR)/data_structures
BUILD_DIR = build

# Source files
SERVER_SOURCES = $(SRC_DIR)/server.c \
                $(SRC_DIR)/safe_socket.c \
                $(SRC_DIR)/server_connection_handler.c \
                $(SRC_DIR)/common_utils.c \
                $(SRC_DIR)/connection_utils.c \
                $(SRC_DIR)/serialization.c \
                $(DATA_STRUCTURES_DIR)/queue.c \
                $(DATA_STRUCTURES_DIR)/cJSON.c \
                $(DATA_STRUCTURES_DIR)/phone_directory.c \
                $(DATA_STRUCTURES_DIR)/hash_map.c

CLIENT_SOURCES = $(SRC_DIR)/client.c \
                $(SRC_DIR)/safe_socket.c \
                $(SRC_DIR)/client_request_handler.c \
                $(SRC_DIR)/common_utils.c \
                $(SRC_DIR)/connection_utils.c \
                $(SRC_DIR)/server_response_parser.c \
                $(DATA_STRUCTURES_DIR)/cJSON.c

# Object files (in build directory)
SERVER_OBJECTS = $(SERVER_SOURCES:%.c=$(BUILD_DIR)/%.o)
CLIENT_OBJECTS = $(CLIENT_SOURCES:%.c=$(BUILD_DIR)/%.o)

# Executables
SERVER = $(BUILD_DIR)/server
CLIENT = $(BUILD_DIR)/client

# Include directories
INCLUDES = -I$(SRC_DIR) -I$(DATA_STRUCTURES_DIR)

# Targets
all: init $(SERVER) $(CLIENT)

# Initialize build directory structure
init:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/$(DATA_STRUCTURES_DIR)

$(SERVER): $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

$(CLIENT): $(CLIENT_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Pattern rule for object files
$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean init