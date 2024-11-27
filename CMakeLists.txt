cmake_minimum_required(VERSION 3.30)
project(phone_dir_app C)

set(CMAKE_C_STANDARD 11)

add_executable( server server.c
        data_structures/queue.h
        data_structures/queue.c
                safe_socket.h
                safe_socket.c)

add_executable(client client.c
                safe_socket.h
                safe_socket.c
        client_request_handler.c
        client_request_handler.h
        data_structures/cJSON.c
        data_structures/cJSON.h)

add_executable(trie_test data_structures/pd_test.c
        data_structures/phone_directory.c
        data_structures/phone_directory.h
        data_structures/hash_map.c
        data_structures/hash_map.h)