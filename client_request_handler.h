#ifndef CLIENT_REQUEST_HANDLER_H
#define CLIENT_REQUEST_HANDLER_H
#define SERVER_PASSWORD "qwerty"

void authentication_procedure();
void modify_procedure();
void consultation_procedure();
void handle_request(int operation, int client_socket);

#endif //CLIENT_REQUEST_HANDLER_H
