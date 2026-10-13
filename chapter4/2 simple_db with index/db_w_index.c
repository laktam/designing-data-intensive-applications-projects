#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct HashMap{
    char *key;
    char *value;
    struct HashMap *next;
} HashMap;

void db_get(char *key);
void db_set(char *key, char *value);
HashMap *init_hashmap();
void put(char *key, char *value, HashMap hashmap[]);
char *get(char *key, HashMap hashmap[]);
int get_index(char *key);
void copy_items(HashMap old[], HashMap new[]);

int capacity = 16;
int size = 0;
float load_factor = 0.75;

int main(int argc, char *argv[]) {
    HashMap *hashmap = init_hashmap();
    put("test", "test value 1", hashmap);
    put("test", "test value 2", hashmap);
    put("test3", "test value3", hashmap);
    printf("value for test : %s\n", get("test", hashmap));
    printf("value for test3 : %s\n", get("test3", hashmap));
    // if(argc == 3) {
    //     if(strcmp(argv[1], "get") == 0) {
    //         db_get(argv[2]);
    //     }
    // } 
    // else if (argc == 4){
    //     if (strcmp(argv[1], "set") == 0){
    //         db_set(argv[2], argv[3]);
    //     }
    // }
}

void db_get(char *key) {
    FILE *fptr = fopen("db.txt", "r");
    if(fptr == NULL) {
        printf("error");
        exit(1);
    }

    char line[100];
    int line_count = 0;
    char *last_key = NULL;

    while(fgets(line, sizeof(line), fptr) != NULL) {
        // check if key is in the start of this line
        for(int i = 0; i < 100; i++) {
            // the key is equal to the start of the line until ';' and the the key ends there
            if(line[i] == ';' && strlen(key) == i) {
                last_key = line + i + 1;
            }
            if(line[i] != key[i]) {
                break;
            }
        }
    }

    if(last_key == NULL) {
        printf("key not found!");
        exit(1);
    }
    printf("%s\n", last_key);
}

void db_set(char *key, char *value) {
    FILE *fptr = fopen("db.txt", "a");
    if(fptr == NULL) {
        printf("error opening file");
        exit(1);
    }
    fprintf(fptr, "%s;%s\n", key, value);
}



HashMap *init_hashmap() {
    HashMap *hashmap = calloc(capacity, sizeof *hashmap);
}

void put(char *key, char *value, HashMap hashmap[]){
    // resize the nodes array if the threashold is reached (0.75 load factor)
    if(++size > capacity * 0.75) { // capacity * load factor = threshold
        capacity = capacity * 2;
        HashMap *new_hashmap = realloc(hashmap, capacity);
        if(new_hashmap == NULL) {
            printf("error reallocating hashmap");
            free(hashmap);
            exit(1);
        }
        copy_items(hashmap, new_hashmap);
        free(hashmap);
        hashmap = new_hashmap;
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

char *get(char *key, HashMap hashmap[]){
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
    }
}

int get_index(char *key) {
    int sum = 0;
    while (*key != '\n'){
        sum += (int) *key;
        key++;
    }
    printf("hash : %d \n", sum % capacity);
    return sum % capacity;
}

void copy_items(HashMap old[], HashMap new[]) {
    for(int i = 0; i < capacity; i++) {
        if(&old[i] != NULL){
            put(old[i].key, old[i].value, new);
            while(old[i].next != NULL){
                put(old[i].next->key, old[i].next->value, new);
                old[i].next = old[i].next->next;
            }
        }
    }
}