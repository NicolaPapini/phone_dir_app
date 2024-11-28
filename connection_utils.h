#ifndef CONNECTION_UTILS_H
#define CONNECTION_UTILS_H
#include "safe_socket.h"
#include "data_structures/cJSON.h"
#include "data_structures/phone_directory.h"

Operation get_operation(char *operation_str);
void get_fields(cJSON *request, char *name, char *surname, char *number, char *new_number);
void add_result_to_response(cJSON *response, Status result);
void create_multiple_entry_jSON(cJSON * json, ListNode * result);

void prompt_fields(char *name, char *surname, char *number, char *new_number);
void build_JSON_request(char *buffer, char *operation, char *name, char *surname, char *number, char *new_number);
void get_input(char *input, int size);

#endif //CONNECTION_UTILS_H
