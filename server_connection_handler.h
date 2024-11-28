#ifndef SERVER_CONNECTION_HANDLER_H
#define SERVER_CONNECTION_HANDLER_H
#include "data_structures/phone_directory.h"

int handle_connection(int client_socket, PhoneDirectory *phone_dir);

#endif //SERVER_CONNECTION_HANDLER_H
