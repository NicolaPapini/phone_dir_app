#include "connection_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common_utils.h"

void free_list_nodes(ListNode * head);

/*############################ Server procedures ############################*/
Operation get_operation(char *operation) {
    if (strcmp(operation, "ADD_CONTACT") == 0) {
        return ADD_CONTACT;
    } else if (strcmp(operation, "DELETE_CONTACT") == 0) {
        return DELETE_CONTACT;
    } else if (strcmp(operation, "UPDATE_CONTACT") == 0) {
        return UPDATE_CONTACT;
    } else if (strcmp(operation, "SEARCH_BY_PREFIX") == 0) {
        return SEARCH_BY_PREFIX;
    } else if (strcmp(operation, "SEARCH_BY_NUMBER") == 0) {
        return SEARCH_BY_NUMBER;
    } else {
        return -1;
    }
}

void get_fields(cJSON *request, char *name, char *surname, char *number, char *new_number) {
    cJSON *name_json = cJSON_GetObjectItem(request, "name");
    cJSON *surname_json = cJSON_GetObjectItem(request, "surname");
    cJSON *number_json = cJSON_GetObjectItem(request, "number");
    cJSON *new_number_json = cJSON_GetObjectItem(request, "new_number");

    if (name_json != NULL) {
        strncpy(name, name_json->valuestring, NAME_SIZE - 1);
        name[NAME_SIZE - 1] = '\0';
    }
    if (surname_json != NULL) {
        strncpy(surname, surname_json->valuestring, SURNAME_SIZE - 1);
        surname[SURNAME_SIZE - 1] = '\0';
    }
    if (number_json != NULL) {
        strncpy(number, number_json->valuestring, NUMBER_SIZE - 1);
        number[NUMBER_SIZE - 1] = '\0';
    }
    if (new_number_json != NULL) {
        strncpy(new_number, new_number_json->valuestring, NUMBER_SIZE - 1);
        new_number[NUMBER_SIZE - 1] = '\0';
    }
}

void add_result_to_response(cJSON *response, Status result) {
    char *to_be_added;
    switch (result) {
        case SUCCESS:
            to_be_added = "SUCCESS";
        break;
        case INVALID_INPUT:
            to_be_added = "INVALID_INPUT";
        break;
        case RECORD_ALREADY_EXISTS:
            to_be_added = "RECORD_ALREADY_EXISTS";
        break;
        case RECORD_NOT_FOUND:
            to_be_added = "RECORD_NOT_FOUND";
        break;
        default:
            to_be_added = "UNKNOWN_ERROR";
    }
    cJSON_AddStringToObject(response, "result", to_be_added);
}

void create_multiple_entry_jSON(cJSON *response, ListNode *result) {
    ListNode *head = result;
    cJSON *jSON_array = cJSON_CreateArray();
    while (result != NULL) {
        cJSON *entry = cJSON_CreateObject();
        cJSON_AddStringToObject(entry, "name", result->contact->name);
        cJSON_AddStringToObject(entry, "surname", result->contact->surname);
        cJSON_AddStringToObject(entry, "number", result->contact->phone_number);
        cJSON_AddItemToArray(jSON_array, entry);
        result = result->next;
    }
    cJSON_AddItemToObject(response, "contacts", jSON_array);
    free_list_nodes(head);
}

void free_list_nodes(ListNode *head) {
    ListNode *current = head;
    while (current != NULL) {
        ListNode *next = current->next;
        free(current);
        current = next;
    }
}
/*############################ Client procedures ############################*/
void prompt_for_input(char *input, int size, char *field_name, bool (*is_valid)(char *), void (*sanitize)(char *)) {
    printf("Enter the %s:\n", field_name);
    get_input(input, size);
    if (sanitize != NULL) {
        sanitize(input);
    }

    while (!is_valid(input)) {
        printf("Invalid %s. Please try again.\n", field_name);
        get_input(input, size);
        if (sanitize != NULL) {
            sanitize(input);
        }
    }
}

void prompt_fields(char *name, char *surname, char *number, char *new_number) {
    if (name != NULL) {
        prompt_for_input(name, NAME_SIZE, "name", is_alpha, sanitize_name_or_surname);
    }

    if (surname != NULL) {
        prompt_for_input(surname, SURNAME_SIZE, "surname", is_alpha, sanitize_name_or_surname);
    }

    if (number != NULL) {
        prompt_for_input(number, NUMBER_SIZE, "number", is_numeric, NULL);
    }

    if (new_number != NULL) {
        prompt_for_input(new_number, NUMBER_SIZE, "new number", is_numeric, NULL);
    }
}

void build_JSON_request(char *buffer, char *operation, char *name, char *surname, char *number, char *new_number) {
    cJSON *request = cJSON_CreateObject();
    cJSON_AddStringToObject(request, "operation", operation);

    if (name != NULL) {
        cJSON_AddStringToObject(request, "name", name);
    }
    if (surname != NULL) {
        cJSON_AddStringToObject(request, "surname", surname);
    }
    if (number != NULL) {
        cJSON_AddStringToObject(request, "number", number);
    }
    if (new_number != NULL) {
        cJSON_AddStringToObject(request, "new_number", new_number);
    }
    strcpy(buffer, cJSON_PrintUnformatted(request));
    cJSON_Delete(request);
}

void get_input(char *buffer, int size) {
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}