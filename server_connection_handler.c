#include "server_connection_handler.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common_utils.h"
#include "connection_utils.h"
#include "safe_socket.h"
pthread_mutex_t phone_dir_mutex = PTHREAD_MUTEX_INITIALIZER;

void server_add(cJSON *request, char *response, PhoneDirectory *phone_dir);
void server_delete(cJSON *request, char *response, PhoneDirectory *phone_dir);
void server_update(cJSON *request, char *response, PhoneDirectory *phone_dir);
void server_search_by_prefix(cJSON *request, char *response, PhoneDirectory *phone_dir);
void server_search_by_number(cJSON *request, char *response, PhoneDirectory *phone_dir);

int handle_connection(int client_socket, PhoneDirectory *phone_dir) {
    char *recvline = calloc(BUFFER_SIZE, sizeof(char));
    ssize_t msg_size = read_message(client_socket, recvline);

    if (msg_size < 0) {
        printf("Error reading from client\n");
        free(recvline);
        return -1;
    }

    if (msg_size == 0) {
        printf("Client disconnected\n");
        free(recvline);
        return 0;
    }

    printf("Received request from socket %d: %s \n", client_socket, recvline);
    cJSON *request = cJSON_Parse(recvline);
    cJSON *operation_json = cJSON_GetObjectItem(request, "operation");
    Operation operation;
    check(operation = get_operation(operation_json->valuestring), "Invalid operation");
    char *response = calloc(BUFFER_SIZE, sizeof(char));

    switch (operation) {
        case ADD_CONTACT:
            server_add(request, response, phone_dir);
            break;
        case DELETE_CONTACT:
            server_delete(request, response, phone_dir);
            break;
        case UPDATE_CONTACT:
            server_update(request, response, phone_dir);
            break;
        case SEARCH_BY_PREFIX:
            server_search_by_prefix(request, response, phone_dir);
            break;
        case SEARCH_BY_NUMBER:
            server_search_by_number(request, response, phone_dir);
            break;
        default:
            printf("Invalid operation\n");
            exit(EXIT_FAILURE);
    }
    printf("Response for socket %d: %s\n", client_socket, response);
    if (write_message(client_socket, response) < 0) {
        printf("Error writing to client\n");
        free(recvline);
        return -1;
    }
    free(response);
    free(recvline);
    return 1;
}

void server_add(cJSON *request, char *response, PhoneDirectory *phone_dir) {
    cJSON *response_jSON = cJSON_CreateObject();
    cJSON_AddStringToObject(response_jSON, "operation", "ADD_CONTACT");
    char name[NAME_SIZE], surname[SURNAME_SIZE], number[NUMBER_SIZE];
    get_fields(request, name, surname, number, NULL);

    pthread_mutex_lock(&phone_dir_mutex);
    Status result = insert_contact(phone_dir, name, surname, number);
    pthread_mutex_unlock(&phone_dir_mutex);

    add_result_to_response(response_jSON, result);
    strcpy(response, cJSON_PrintUnformatted(response_jSON));
    cJSON_Delete(response_jSON);
}

void server_delete(cJSON *request, char *response, PhoneDirectory *phone_dir) {
    cJSON *response_jSON = cJSON_CreateObject();
    cJSON_AddStringToObject(response_jSON, "operation", "DELETE_CONTACT");
    char name[NAME_SIZE], surname[SURNAME_SIZE], number[NUMBER_SIZE];
    get_fields(request, name, surname, number, NULL);

    pthread_mutex_lock(&phone_dir_mutex);
    Status result = delete_contact(phone_dir, name, surname, number);
    pthread_mutex_unlock(&phone_dir_mutex);

    add_result_to_response(response_jSON, result);
    strcpy(response, cJSON_PrintUnformatted(response_jSON));
    cJSON_Delete(response_jSON);
}

void server_update(cJSON *request, char *response, PhoneDirectory *phone_dir) {
    cJSON *response_jSON = cJSON_CreateObject();
    cJSON_AddStringToObject(response_jSON, "operation", "UPDATE_CONTACT");
    char number[NUMBER_SIZE], new_number[NUMBER_SIZE];
    get_fields(request, NULL, NULL, number, new_number);

    pthread_mutex_lock(&phone_dir_mutex);
    Status result = update_contact_number(phone_dir, number, new_number);
    pthread_mutex_unlock(&phone_dir_mutex);

    add_result_to_response(response_jSON, result);
    strcpy(response, cJSON_PrintUnformatted(response_jSON));
    cJSON_Delete(response_jSON);
}

void server_search_by_prefix(cJSON *request, char *response, PhoneDirectory *phone_dir) {
    cJSON *response_jSON = cJSON_CreateObject();
    cJSON_AddStringToObject(response_jSON, "operation", "SEARCH_BY_PREFIX");
    char name[NAME_SIZE], surname[SURNAME_SIZE];
    get_fields(request, name, surname, NULL, NULL);

    ListNode *result = search_contacts_by_prefix(phone_dir, name, surname);

    if (result == NULL) {
        add_result_to_response(response_jSON, RECORD_NOT_FOUND);
    } else {
        add_result_to_response(response_jSON, SUCCESS);
        create_multiple_entry_jSON(response_jSON, result);
    }
    char *str = cJSON_PrintUnformatted(response_jSON);
    printf("Response: %s\n", str);
    strcpy(response, cJSON_PrintUnformatted(response_jSON));
    cJSON_Delete(response_jSON);
}

void server_search_by_number(cJSON *request, char *response, PhoneDirectory *phone_dir) {
    cJSON *response_jSON = cJSON_CreateObject();
    cJSON_AddStringToObject(response_jSON, "operation", "SEARCH_BY_NUMBER");
    char number[NUMBER_SIZE];
    get_fields(request, NULL, NULL, number, NULL);

    Contact *result = search_contact_by_phone_number(phone_dir, number);

    if (result == NULL) {
        add_result_to_response(response_jSON, RECORD_NOT_FOUND);
    } else {
        add_result_to_response(response_jSON, SUCCESS);
        cJSON_AddStringToObject(response_jSON, "name", result->name);
        cJSON_AddStringToObject(response_jSON, "surname", result->surname);
        cJSON_AddStringToObject(response_jSON, "number", result->phone_number);
    }
    strcpy(response, cJSON_PrintUnformatted(response_jSON));
    cJSON_Delete(response_jSON);
}