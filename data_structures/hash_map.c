#include "hash_map.h"
#include "phone_directory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hash(char *key, int capacity);
void rehash(HashMap *hash_map);

void print_hash_map(HashMap *hash_map) {
    if (hash_map == NULL) {
        printf("Hash map is NULL\n");
        return;
    }

    for (int i = 0; i < hash_map->capacity; i++) {

        ListNode *current = hash_map->buckets[i];
        if (current != NULL) {
            printf("Bucket %d: ", i);
            while (current != NULL) {
                printf("-> [Name: %s, Surname: %s, Number: %s] ", current->contact->name, current->contact->surname, current->contact->phone_number);
                current = current->next;
            }
            printf("\n");
        }
    }
}

/*###################### Constructors ###################### */
HashMap *create_hash_map() {
    HashMap *hash_map = (HashMap*) malloc(sizeof(HashMap));
    hash_map->size = 0;
    hash_map->capacity = INITIAL_CAPACITY;
    hash_map->buckets = (ListNode**) malloc(sizeof(ListNode*) * INITIAL_CAPACITY);
    for (int i = 0; i < INITIAL_CAPACITY; i++) {
        hash_map->buckets[i] = NULL;
    }
    return hash_map;
}

/*###################### Helpers ###################### */
int hash(char *key, int capacity) {
    char *end_ptr;
    long hash_value = strtol(key, &end_ptr, 10);
    return (int) (hash_value % capacity);
}

//TODO: maybe rehash when the load factor is too low

void rehash(HashMap *hash_map) {
    int new_capacity = hash_map->capacity * 2;
    ListNode **new_buckets = (ListNode**) malloc(sizeof(ListNode*) * new_capacity);

    for (int i = 0; i < new_capacity; i++) {
        new_buckets[i] = NULL;
    }

    for (int i = 0; i < hash_map->capacity; i++) {
        ListNode *current = hash_map->buckets[i];
        while (current != NULL) {
            ListNode *next = current->next;
            int new_bucket = hash(current->contact->phone_number, new_capacity);
            current->next = new_buckets[new_bucket];
            new_buckets[new_bucket] = current;
            current = next;
        }
    }
    free(hash_map->buckets);
    hash_map->buckets = new_buckets;
    hash_map->capacity = new_capacity;
}

/*###################### Functions ###################### */

void put_entry(HashMap *hash_map, Contact *value) {
    if (hash_map == NULL || value == NULL) {
        return;
    }

    int bucket = hash(value->phone_number, hash_map->capacity);
    ListNode *new_node = create_list_node(value);
    if (hash_map->buckets[bucket] != NULL) {
        new_node->next = hash_map->buckets[bucket];
    }
    hash_map->buckets[bucket] = new_node;
    hash_map->size++;

    if (hash_map->size >= hash_map->capacity * MAX_LOAD_FACTOR) {
        rehash(hash_map);
    }
}

Contact *get(HashMap *hash_map, char *key) {
    if (hash_map == NULL || key == NULL) {
        return NULL;
    }
    int bucket = hash(key, hash_map->capacity);
    ListNode *current = hash_map->buckets[bucket];
    while (current != NULL) {
        if (strcmp(current->contact->phone_number, key) == 0) {
            return current->contact;
        }
        current = current->next;
    }
    return NULL;
}

void remove_entry(HashMap *map, char *key) {
    if (map == NULL || key == NULL) {
        return;
    }

    int bucket = hash(key, map->capacity);
    ListNode *current = map->buckets[bucket];
    ListNode *prev = NULL;
    while (current != NULL) {
        if (strcmp(current->contact->phone_number, key) == 0) {
            if (prev == NULL) {
                map->buckets[bucket] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            map->size--;
            return;
        }
        prev = current;
        current = current->next;
    }
}

void free_hash_map(HashMap *hash_map) {
    if (hash_map == NULL) {
        return;
    }

    for (int i = 0; i < hash_map->capacity; i++) {
        ListNode *current = hash_map->buckets[i];
        while (current != NULL) {
            ListNode *next = current->next;
            free(current);
            current = next;
        }
    }
    free(hash_map->buckets);
    free(hash_map);
}