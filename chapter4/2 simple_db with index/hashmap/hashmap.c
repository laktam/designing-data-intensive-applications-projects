#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "hashmap.h"

static void copy_items(HashMap* old_hashmap_container, HashMapNode *new_hashmap);
static int get_index(char *key, HashMap* hashmap_container);

HashMap* create_hashmap(int capacity, float load_factor) {
    HashMap* hashmap = malloc(sizeof(*hashmap));
    HashMapNode* hashmap_head_p = calloc(capacity, sizeof(HashMapNode));
    if(hashmap_head_p == NULL || hashmap == NULL){
        printf("can't create hashmap");
        exit(1);
    }
    hashmap->head_p = hashmap_head_p;
    hashmap->capacity = capacity;
    hashmap->size = 0;
    hashmap->load_factor = load_factor;
    return hashmap;
}

HashMap* create_default_hashmap() {
    return create_hashmap(16, 0.75);
}

void put(char *key, long value, HashMap* hashmap_container){
    HashMapNode* hashmap = hashmap_container->head_p; 
    char* key_dup = strdup(key);
    if(key_dup == NULL) {
        exit(1);
    }

    // hash the key and use the hash it as index
    // insert if if the place is empty or else put it in next
    int index = get_index(key_dup, hashmap_container);
    if(hashmap[index].key == NULL) {
        hashmap[index] = (HashMapNode){
            .key = key_dup,
            .value = value,
            .next = NULL
        };
    }
    else {
        // go until next is null then place the node there
        // or replace the value if keys are identical (starting with the first one in the linked list)
        HashMapNode *current = &hashmap[index];
        HashMapNode* before_current = current;
        while(current != NULL) {
            if(strcmp(current->key, key) == 0) {
                current->value = value;
                free(key_dup);
                return;
            }
            before_current = current;
            current = current->next;
        }
        current = malloc(sizeof *hashmap);
        if(current == NULL) {
            free(key_dup);
            exit(1);
        }
        current->key = key_dup;
        current->value = value;
        current->next = NULL;
        before_current->next = current; 
    }

    if(hashmap_container->size != -1){
        hashmap_container->size++;
    }

    // resize the nodes array if the threashold is reached (load factor)
    // size -1 means we are in a resizing process
    if(hashmap_container->size != -1 && hashmap_container->size > hashmap_container->capacity * hashmap_container->load_factor) { // internal_capacity * load factor = threshold
        HashMapNode *new_hashmap = calloc(hashmap_container->capacity * 2, sizeof(HashMapNode));
        if(new_hashmap == NULL) {
            printf("error reallocating hashmap");
            free(hashmap);
            exit(1);
        }
        copy_items(hashmap_container, new_hashmap);
        destroy_hashmap_content(hashmap, hashmap_container->capacity / 2);
        hashmap = hashmap_container->head_p;
    }

}

long get(char *key, HashMap* hashmap_container){
    HashMapNode* hashmap = hashmap_container->head_p;
    int index = get_index(key, hashmap_container);
    if(hashmap[index].key == NULL) {
        return -1L;
    }

    if(strcmp(hashmap[index].key, key) == 0) {
        return hashmap[index].value;
    }
    else {
        HashMapNode *current = hashmap[index].next;
        while(current != NULL) {
            if(strcmp(current->key, key) == 0) {
                return current->value;
            }
            else {
                current = current->next; 
            }
        }
        return -1L;// not found in the hashmap
    }
}

static int get_index(char *key, HashMap* hashmap_container) {
    int sum = 0;
    while (*key != '\0'){
        sum += (int) *key;
        key++;
    }
    printf("hash : %d \n", sum % hashmap_container->capacity);
    return sum % hashmap_container->capacity;
}

static void copy_items(HashMap* old_hashmap_container, HashMapNode *new_hashmap) {
    HashMapNode* old_hashmap = old_hashmap_container->head_p;
    old_hashmap_container->head_p = new_hashmap;
    old_hashmap_container->capacity = old_hashmap_container->capacity * 2;
    old_hashmap_container->size = -1;

    int size = 0;
    for(int i = 0; i < old_hashmap_container->capacity / 2; i++) {
        if(old_hashmap[i].key != NULL){
            put(old_hashmap[i].key, old_hashmap[i].value, old_hashmap_container);
            size++;
            while(old_hashmap[i].next != NULL){
                put(old_hashmap[i].next->key, old_hashmap[i].next->value, old_hashmap_container);
                HashMapNode* tmp = old_hashmap[i].next;
                old_hashmap[i].next = old_hashmap[i].next->next;
                // free this node because we lose pointer to it 
                free(tmp->key);
                free(tmp);
                size++;
            }
        }
    }
    old_hashmap_container->size = size;
}

static void destroy_hashmap_content(HashMapNode *hashmap, int capacity) {
    for(int i = 0; i < capacity; i++) {
        HashMapNode *current = &hashmap[i];
        bool first = true;
        while(current != NULL) {
            free(current->key);
            HashMapNode *tmp = current->next;
            if(!first) {
                free(current);
            }
            current = tmp;
            first = false;
        }
    }
    free(hashmap);
}

void destroy_hashmap(HashMap* hashmap) {
    destroy_hashmap_content(hashmap->head_p, hashmap->capacity);
    free(hashmap);
}