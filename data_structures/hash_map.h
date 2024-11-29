#ifndef HASH_MAP_H
#define HASH_MAP_H

#define INITIAL_CAPACITY 16
#define MAX_LOAD_FACTOR 0.75

typedef struct Contact Contact;
typedef struct ListNode ListNode;

typedef struct HashMap {
    int size;
    int capacity;
    ListNode **buckets;
} HashMap;

HashMap *create_hash_map();
void put_entry(HashMap *hash_map, Contact *value);
Contact *get(HashMap *hash_map, char *key);
void remove_entry(HashMap *map, char *key);
void free_hash_map(HashMap *hash_map);

#endif //HASH_MAP_H
