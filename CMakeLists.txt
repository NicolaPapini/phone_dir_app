cmake_minimum_required(VERSION 3.30)
project(phone_dir_app C)

set(CMAKE_C_STANDARD 11)

add_executable( server server.c
        safe_socket.c
        common_utils.c
        connection_utils.c
        server_connection_handler.c
        serialization.c
        data_structures/cJSON.c
        data_structures/queue.c
        data_structures/phone_directory.c
        data_structures/hash_map.c)

add_executable(client client.c
                safe_socket.c
                client_request_handler.c
                data_structures/cJSON.c
                common_utils.c
                connection_utils.c
        server_response_parser.c)
