#include "server_response_parser.h"

#include <stdio.h>
#include <string.h>

#include "connection_utils.h"

void print_contact(cJSON *contact);
void print_all_contacts(cJSON *contacts);

void parse_response(cJSON *response) {
    cJSON *operation_json = cJSON_GetObjectItem(response, "operation");
    cJSON *result_json = cJSON_GetObjectItem(response, "result");

    char *operation_str = operation_json->valuestring;
    Operation operation = get_operation(operation_str);
    char *result = result_json->valuestring;

    printf("%s: %s\n", operation_str, result);

    switch (operation) {
        case SEARCH_BY_PREFIX:
            if (strcmp(result, "SUCCESS") == 0) {
                cJSON *contacts = cJSON_GetObjectItem(response, "contacts");
                print_all_contacts(contacts);
            }
            break;
        case SEARCH_BY_NUMBER:
            if (strcmp(result, "SUCCESS") == 0) {
                print_contact(response);
            }
            break;
        default:
            break;
    }
    printf("\n=============================================================================================\n");
}

void print_contact(cJSON *contact) {
    const char *name = cJSON_GetObjectItem(contact, "name")->valuestring;
    const char *surname = cJSON_GetObjectItem(contact, "surname")->valuestring;
    const char *number = cJSON_GetObjectItem(contact, "number")->valuestring;
    printf("\n==============================\n");
    printf("   Contact Information\n");
    printf("==============================\n");
    printf("Name   : %s\n", name);
    printf("Surname: %s\n", surname);
    printf("Number : %s\n", number);
    printf("==============================\n\n");
}

void print_all_contacts(cJSON *contacts) {
    int size = cJSON_GetArraySize(contacts);
    printf("\n==============================\n");
    printf("   Contacts\n");
    printf("==============================\n");
    for (int i = 0; i < size; i++) {
        cJSON *contact = cJSON_GetArrayItem(contacts, i);
        print_contact(contact);
        if (i < size - 1) {
            printf("\n");
        }
    }
    printf("==============================\n");
}


