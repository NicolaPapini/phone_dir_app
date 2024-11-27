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
                safe_socket.c)

