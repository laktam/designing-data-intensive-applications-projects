#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "hashmap.h"

static int internal_capacity = 16;
static int size = 0; //!!! size is global  to all hashmaps wich is wrong
static float internal_load_factor = 0.75;

typedef struct HashMap{
    char *key;
    long value;
    struct HashMap *next;
} HashMap;

HashMap *create_hashmap(int capacity, int load_factor) {
    internal_capacity= capacity;
    internal_load_factor = load_factor;
    HashMap *hashmap = calloc(capacity, sizeof(HashMap));
    return hashmap;
}

HashMap *create_default_hashmap() {
    HashMap *hashmap = calloc(internal_capacity, sizeof(HashMap));
    return hashmap;
}

void put(char *key, long value, HashMap** hashmap_p){
    HashMap* hashmap = *hashmap_p;
    key = strdup(key);
    if(key == NULL) {
        exit(1);
    }
    // resize the nodes array if the threashold is reached (0.75 load factor)
    if(++size > internal_capacity * 0.75) { // internal_capacity * load factor = threshold
        internal_capacity = internal_capacity * 2;
        HashMap *new_hashmap = malloc(internal_capacity * sizeof(HashMap));
        if(new_hashmap == NULL) {
            printf("error reallocating hashmap");
            free(hashmap);
            exit(1);
        }
        copy_items(hashmap, new_hashmap);
        *hashmap_p = new_hashmap;
        destroy_hashmap(hashmap);
        hashmap = new_hashmap;
        size++;// because we reset size in copy_items
    }
    // hash the key and use the hash it as index
    // insert if if the place is empty or else put it in next
    int index = get_index(key);
    if(hashmap[index].key == NULL) {
        hashmap[index] = (HashMap){
            .key = key,
            .value = value,
            .next = NULL
        };
    }
    else {
        // go until next is null then place the node there
        // or replace the value if keys are identical (starting with the first one in the linked list)
        HashMap *current = &hashmap[index];
        while(current != NULL) {
            if(strcmp(current->key, key) == 0) {
                current->key = key;
                current->value = value;
                return;
            }
            current = current->next;
        }
        current = malloc(sizeof *hashmap);
        current->key = key;
        current->value = value;
        current->next = NULL;
    }
    
}

long get(char *key, HashMap *hashmap){
    int index = get_index(key);
    if(strcmp(hashmap[index].key, key) == 0) {
        return hashmap[index].value;
    }
    else {
        HashMap *current = hashmap[index].next;
        while(current != NULL) {
            if(strcmp(current->next->key, key) == 0) {
                return current->value;
            }
            else {
                current->next = current->next->next; 
            }
        }
        return -1L;// not found in the hashmap
    }
}

int get_index(char *key) {
    int sum = 0;
    while (*key != '\0'){
        sum += (int) *key;
        key++;
    }
    printf("hash : %d \n", sum % internal_capacity);
    return sum % internal_capacity;
}

void copy_items(HashMap *old_hashmap, HashMap *new_hashmap) {
    size = 0; // reset size to not double it, because put is called here
    for(int i = 0; i < internal_capacity / 2; i++) {
        if(old_hashmap[i].key != NULL){
            put(old_hashmap[i].key, old_hashmap[i].value, &new_hashmap);
            while(old_hashmap[i].next != NULL){
                put(old_hashmap[i].next->key, old_hashmap[i].next->value, &new_hashmap);
                old_hashmap[i].next = old_hashmap[i].next->next;
            }
        }
    }
}

void destroy_hashmap(HashMap *hashmap) {
    for(int i = 0; i < internal_capacity; i++) {
        HashMap *current = &hashmap[i];
        bool first = true;
        while(current != NULL) {
            free(current->key);
            HashMap *tmp = current->next;
            if(!first) {
                free(current);
            }
            current = tmp;
            first = false;
        }
    }
    free(hashmap);
}