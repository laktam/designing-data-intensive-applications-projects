#ifndef HASHMAP_H
#define HASHMAP_H

typedef struct HashMapNode{
    char *key;
    long value;
    struct HashMapNode *next;
} HashMapNode;

typedef struct HashMap{
    HashMapNode* head_p;
    int capacity;
    int size;
    float load_factor;
} HashMap;

HashMap* create_default_hashmap();
HashMap* create_hashmap(int capacity, float load_factor);
void put(char *key, long value, HashMap* hashmap_container);
long get(char *key, HashMap* hashmap_container);
void destroy_hashmap_chain(HashMapNode *hashmap, int capacity);
void destroy_hashmap(HashMap* hashmap);

#endif