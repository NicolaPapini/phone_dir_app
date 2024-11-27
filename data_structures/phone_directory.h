#ifndef PHONE_DIRECTORY_H
#define PHONE_DIRECTORY_H

#include <stdbool.h>
#include "hash_map.h"

#define ALPHABET_SIZE 27
#define ALPHABET "abcdefghijklmnopqrstuvwxyz "

//TODO Implement ListNode instead of Contact

struct HashMap;
typedef struct Contact{
    char *name;
    char *surname;
    char *phone_number;
} Contact;

typedef struct ListNode {
    Contact *contact;
    struct ListNode *next;
} ListNode;

typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    ListNode *head;
    bool is_end_of_word;
} TrieNode;

typedef struct PhoneDirectory{
    TrieNode *root;
    struct HashMap *hash_map;
} PhoneDirectory;

typedef enum Status {
    SUCCESS,
    INVALID_INPUT,
    RECORD_ALREADY_EXISTS,
    RECORD_NOT_FOUND
} Status;

PhoneDirectory *create_phone_directory();
ListNode *create_list_node(Contact *contact);
Status insert_contact(PhoneDirectory *phone_directory, char *name, char *surname, char *phone_number);
Status delete_contact(PhoneDirectory *phone_directory, char *name, char *surname, char *phone_number);
ListNode *search_contacts_by_prefix(PhoneDirectory *phone_directory, char *name, char *surname);
Contact *search_contact_by_phone_number(PhoneDirectory *phone_directory, char *phone_number);
Status update_contact_number(PhoneDirectory *phone_directory, char *phone_number, char *new_phone_number);
void free_phone_directory(PhoneDirectory *phone_directory);
#endif //PHONE_DIRECTORY_H
