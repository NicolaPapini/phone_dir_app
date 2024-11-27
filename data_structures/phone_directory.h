#ifndef PHONE_DIRECTORY_H
#define PHONE_DIRECTORY_H

#include <stdbool.h>

#define ALPHABET_SIZE 27
#define ALPHABET "abcdefghijklmnopqrstuvwxyz "

typedef struct Contact{
    char *name;
    char *surname;
    char *phone_number;
    struct Contact *next;
} Contact;

typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    Contact *head;
    bool is_end_of_word;
} TrieNode;

typedef struct PhoneDirectory{
    TrieNode *root;
} PhoneDirectory;

typedef enum Status {
    SUCCESS,
    INVALID_INPUT,
    RECORD_ALREADY_EXISTS,
    RECORD_NOT_FOUND
} Status;

PhoneDirectory *create_phone_directory();
Contact *create_contact(char *name, char *surname, char *phone_number);
Status insert_contact(PhoneDirectory *phone_directory, char *name, char *surname, char *phone_number);
Status delete_contact(PhoneDirectory *phone_directory, char *name, char *surname);
Contact *find_contact_by_prefix(PhoneDirectory *phone_directory, char *name, char *surname);
Contact *find_contact_by_phone_number(PhoneDirectory *phone_directory, char *phone_number);
Contact *update_contact(PhoneDirectory *phone_directory, char *name, char *surname, char *phone_number);
#endif //PHONE_DIRECTORY_H
