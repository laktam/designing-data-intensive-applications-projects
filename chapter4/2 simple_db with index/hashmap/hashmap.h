#ifndef HASHMAP_H
#define HASHMAP_H

typedef struct HashMap HashMap;
HashMap *create_default_hashmap();
HashMap *create_hashmap(int capacity, int load_factor);
void put(char *key, char *value, HashMap *hashmap);
char *get(char *key, HashMap *hashmap);
int get_index(char *key);
void copy_items(HashMap *old_hashmap, HashMap *new_hashmap);

#endif