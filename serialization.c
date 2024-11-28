#include "serialization.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *create_db_entry(Contact *contact);
bool serialize_trie(FILE *file, TrieNode *node);

bool serialize(PhoneDirectory *phone_dir) {
    FILE *file = fopen(PHONE_DIRECTORY_FILE, "w");
    if (file == NULL) {
        return false;
    }

    serialize_trie(file, phone_dir->root);
    fclose(file);
    return true;
}

void deserialize(PhoneDirectory *phone_dir) {
    FILE *file = fopen(PHONE_DIRECTORY_FILE, "r");
    if (file == NULL) {
        return;
    }

    char line[MAX_LINE];
    while (fgets(line, MAX_LINE, file)) {
        char *name = strtok(line, ",");
        char *surname = strtok(NULL, ",");
        char *phone_number = strtok(NULL, ",");
        phone_number[strlen(phone_number) - 1] = 0;
        insert_contact(phone_dir, name, surname, phone_number);
    }
    fclose(file);
}

bool serialize_trie(FILE *file, TrieNode *node) {
    if (node == NULL) {
        return false;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        serialize_trie(file, node->children[i]);
    }

    if (node->is_end_of_word && node->head != NULL) {
        ListNode *current = node->head;
        while (current != NULL) {
            char *entry = create_db_entry(current->contact);
            fwrite(entry, strlen(entry), 1, file);
            current = current->next;
        }
    }
    return true;
}

char* create_db_entry(Contact *contact) {
    int totalLength = strlen(contact->name) + strlen(contact->surname) + strlen(contact->phone_number) + 3 + 1;
    char *entry = (char *)malloc(totalLength * sizeof(char));
    snprintf(entry, totalLength, "%s,%s,%s\n", contact->name, contact->surname, contact->phone_number);
    return entry;
}