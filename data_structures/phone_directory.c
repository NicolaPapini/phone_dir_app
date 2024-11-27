#include "phone_directory.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

bool is_numeric(char *number);
char *standardize(char *name, char *surname);
int get_index(char c);
void find_contacts_with_same_prefix(TrieNode *node, ListNode **result_list);
bool isEmpty(TrieNode node);

/*###################### Constructors ###################### */
TrieNode *create_trie_node() {
    TrieNode *node = (TrieNode*) malloc(sizeof(TrieNode));
    node->is_end_of_word = false;
    node->head = NULL;
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        node->children[i] = NULL;
    }
    return node;
}

Contact *create_contact(char *name, char *surname, char *phone_number) {
    Contact *contact = (Contact*) malloc(sizeof(Contact));
    contact->name = name;
    contact->surname = surname;
    contact->phone_number = phone_number;
    return contact;
}

PhoneDirectory *create_phone_directory() {
    PhoneDirectory *phone_directory = (PhoneDirectory*) malloc(sizeof(PhoneDirectory));
    phone_directory->root = create_trie_node();
    phone_directory->hash_map = create_hash_map();
    return phone_directory;
}

ListNode *create_list_node(Contact *contact) {
    ListNode *node = (ListNode*) malloc(sizeof(ListNode));
    node->contact = contact;
    node->next = NULL;
    return node;
}

/*####################### Functions ####################### */
Status insert_contact(PhoneDirectory *phone_directory, char *surname, char *name, char *phone_number) {
    if (phone_directory == NULL ||
        surname == NULL ||
        name == NULL ||
        phone_number == NULL ||
        !is_numeric(phone_number)) {
        return INVALID_INPUT;
    }

    if (get(phone_directory->hash_map, phone_number) != NULL) {
        return RECORD_ALREADY_EXISTS;
    }

    char *standardized = standardize(name, surname);

    TrieNode *current = phone_directory->root;

    for (int i = 0; i < strlen(standardized); i++) {
        int index = get_index(standardized[i]);
        if (current->children[index] == NULL) {
            current->children[index] = create_trie_node();
        }
        current = current->children[index];
    }

    current->is_end_of_word = true;

    Contact *contact = create_contact(name, surname, phone_number);
    ListNode *node = create_list_node(contact);

    if (current->head == NULL) {
        current->head = node;
    } else {
        node->next = current->head;
        current->head = node;
    }

    put_entry(phone_directory->hash_map, contact);
    free(standardized);

    return SUCCESS;
}

Status delete_contact(PhoneDirectory *phone_dir, char *name, char *surname, char *phone_number) {

    if (phone_dir == NULL || surname == NULL || name == NULL || phone_number == NULL || !is_numeric(phone_number)) {
        return INVALID_INPUT;
    }

    if (get(phone_dir->hash_map, phone_number) == NULL) {
        return RECORD_NOT_FOUND;
    }

    char *standardized_name = standardize(name, surname);
    TrieNode *current = phone_dir->root;
    TrieNode *stack[strlen(standardized_name)];
    int stack_indices[strlen(standardized_name)];
    int stack_index = 0;

    // Traverse the Trie
    for (int i = 0; i < strlen(standardized_name); i++) {
        int index = get_index(standardized_name[i]);
        //TODO: this is redundant
        if (current->children[index] == NULL) {
            free(standardized_name);
            return RECORD_NOT_FOUND;
        }
        stack[stack_index] = current;       // Push current node to stack
        stack_indices[stack_index++] = index; // Push current index to stack_indices
        current = current->children[index];
    }

    free(standardized_name);
    //TODO: this is redundant
    if (!current->is_end_of_word) {
        return RECORD_NOT_FOUND;
    }

    // Remove the contact from the linked list
    ListNode *temp = current->head;
    ListNode *prev = NULL;
    while (temp != NULL) {
        if (strcmp(temp->contact->phone_number, phone_number) == 0) {
            if (prev == NULL) {
                current->head = temp->next;
            } else {
                prev->next = temp->next;
            }

            remove_entry(phone_dir->hash_map, phone_number);
            free(temp->contact);
            free(temp);
            // If no more contacts, mark as not end of word
            if (current->head == NULL) {
                current->is_end_of_word = false;
            }
            break;
        }
        prev = temp;
        temp = temp->next;
    }

    // If the phone number was not found in the linked list
    if (temp == NULL) {
        return RECORD_NOT_FOUND;
    }

    // Backtrack to clean up the Trie
    for (int i = stack_index - 1; i >= 0; i--) {
        int child_index = stack_indices[i]; // Retrieve index of the current child
        TrieNode *parent = stack[i];

        if (isEmpty(*current) && !current->is_end_of_word) {
            free(current);                  // Free the empty TrieNode
            parent->children[child_index] = NULL; // Remove reference in the parent node
        } else {
            break; // Stop backtracking if current node has children or is an end-of-word
        }
        current = parent;
    }
    return SUCCESS;
}

ListNode *search_contacts_by_prefix(PhoneDirectory *phone_directory, char *name, char *surname) {
    if (phone_directory == NULL || surname == NULL || name == NULL) {
        return NULL;
    }

    char *standardized = standardize(name, surname);
    TrieNode *current = phone_directory->root;

    for (int i = 0; i < strlen(standardized); i++) {
        int index = get_index(standardized[i]);
        if (current->children[index] == NULL) {
            free(standardized);
            return NULL;
        }
        current = current->children[index];
    }

    free(standardized);

    // Collect contacts into a linked list
    ListNode *result_list = NULL;
    find_contacts_with_same_prefix(current, &result_list);
    return result_list;
}

Contact *search_contact_by_phone_number(PhoneDirectory *phone_directory, char *phone_number) {
    if (phone_directory == NULL || phone_number == NULL || !is_numeric(phone_number)) {
        return NULL;
    }
    return get(phone_directory->hash_map, phone_number);
}

Status update_contact(PhoneDirectory *phone_directory, char *name, char *surname, char *phone_number, char *new_phone_number) {
    if (phone_directory == NULL ||
        surname == NULL ||
        name == NULL ||
        phone_number == NULL ||
        new_phone_number == NULL ||
        !is_numeric(phone_number) ||
        !is_numeric(new_phone_number)) {
        return INVALID_INPUT;
    }

    Contact *contact = get(phone_directory->hash_map, phone_number);
    if (contact == NULL) {
        return RECORD_NOT_FOUND;
    }
    remove_entry(phone_directory->hash_map, phone_number);
    contact->phone_number = new_phone_number;
    put_entry(phone_directory->hash_map, contact);
    return SUCCESS;
}

void free_phone_directory(PhoneDirectory *phone_directory) {}
    //TODO implement freeing of the trie
    //TODO implement freeing of the hashmap
/*###################### Helpers ###################### */

bool is_numeric(char *number) {
    for (int i = 0; number[i] != '\0'; i++) {
        if (!isdigit(number[i])) {
            return false;
        }
    }
    return true;
}

char *standardize(char *name, char *surname) {
    int len = strlen(name) + strlen(surname) + 1;
    char *standardized = (char*) malloc(sizeof(char) * (len + 1));
    int i = 0;
    for (int j = 0; surname[j]; j++) {
        standardized[i++] = tolower(surname[j]);
    }
    for (int j = 0; name[j]; j++) {
        standardized[i++] = tolower(name[j]);
    }
    standardized[i] = '\0';
    return standardized;
}

int get_index(char c) {
    if (c == ' ') {
        return 26;
    }
    return c - 'a';
}

//TODO: Remember to free the memory allocated for the new list
/*
void free_list_nodes(ListNode *head) {
    ListNode *current = head;
    while (current != NULL) {
        ListNode *next = current->next;
        free(current);
        current = next;
    }
}
*/
void find_contacts_with_same_prefix(TrieNode *node, ListNode **result_list) {
    if (node == NULL) {
        return;
    }
    if (node->is_end_of_word) {
        ListNode *current = node->head;
        while (current != NULL) {
            ListNode *new_node = create_list_node(current->contact);
            new_node->next = *result_list;
            *result_list = new_node;
            current = current->next;
        }
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        find_contacts_with_same_prefix(node->children[i], result_list);
    }
}

bool isEmpty(TrieNode node) {

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node.children[i] != NULL) {
            return false;
        }
    }
    return true;
}