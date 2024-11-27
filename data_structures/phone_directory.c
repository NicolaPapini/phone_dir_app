#include "phone_directory.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

bool is_numeric(char *number);
char *standardize(char *name, char *surname);
int get_index(char c);
Contact *find_contacts_with_same_prefix(TrieNode *node, Contact *head);

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
    contact->next = NULL;
    return contact;
}

PhoneDirectory *create_phone_directory() {
    PhoneDirectory *phone_directory = (PhoneDirectory*) malloc(sizeof(PhoneDirectory));
    phone_directory->root = create_trie_node();
    return phone_directory;
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

    //TODO: Add check for record already existing in the hashmap
    //TODO: Insert the record in the hashmap

    char *standardized = standardize(name, surname);

    TrieNode *current = phone_directory->root;

    for (int i = 0; strlen(standardized); i++) {
        int index = get_index(standardized[i]);
        if (current->children[index] == NULL) {
            current->children[index] = create_trie_node();
        }
        current = current->children[index];
    }

    free(standardized);
    current->is_end_of_word = true;
    Contact *contact = create_contact(name, surname, phone_number);

    if (current->head == NULL) {
        current->head = contact;
    } else {
        contact->next = current->head;
        current->head = contact;
    }

    return SUCCESS;
}


Contact *search_contact_by_prefix(PhoneDirectory *phone_directory, char *name, char *surname) {
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
    Contact *head = NULL;
    return find_contacts_with_same_prefix(current, head);
}

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
Contact *find_contacts_with_same_prefix(TrieNode *node, Contact *head) {
    if (node == NULL) {
        return head;
    }
    if (node->is_end_of_word) {
        Contact *contact = node->head;
        while (contact != NULL) {
            Contact *new_contact = create_contact(contact->name, contact->surname, contact->phone_number);
            new_contact->next = head;
            head = new_contact;
            contact = contact->next;
        }
    }
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        head = find_contacts_with_same_prefix(node->children[i], head);
    }
    return head;
}