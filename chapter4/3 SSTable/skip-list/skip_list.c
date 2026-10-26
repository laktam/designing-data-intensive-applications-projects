#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include "skip_list.h"

SkipList* create_skip_list(int max_level, size_t max_size) {
    if(max_level <= 0) {
        printf("max levels need to be greated than 0");
        return NULL;
    }
    // initialize randomness once in skip list creation
    srand(time(NULL));
    SkipList* skip_list = malloc(sizeof(*skip_list));
    if(skip_list == NULL) {
        printf("error creating skip list");
        exit(1);
    }
    
    // size of skip list stsruct
    size_t size = sizeof(SkipList);
    // size of head (the struct + dynamic nexts array)
    size += sizeof(SkipListNode) + sizeof(SkipListNode*) * max_level;
    printf("size of creation %d\n", size);
    
    skip_list->head = createSkipListNode(NULL, NULL, max_level);
    skip_list->max_levels = max_level;
    skip_list->size = size;
    skip_list->max_size = max_size;
    return skip_list;
}

SkipListNode* createSkipListNode(char* key, char* value, int max_level) {
    // space for head node + spacde for array pointers to max_levels nodes
    SkipListNode* new_node = malloc(sizeof(SkipListNode) + max_level * sizeof(SkipListNode*));
    if(new_node == NULL) {
        printf("error creating a new node"); 
        exit(1);
    }
    new_node->key = key;
    new_node->value = value;
    for(int i = 0; i < max_level; i++) {
        new_node->nexts[i] = NULL;
    }
    return new_node;
}

/*
- find the predecessor for the key on each level
    - travers each level until the next key is bigger
    - go down and traverse
    - repeat till the last level
- randomly decide the levels it will participate in
- make new node point to predecessor next node at each level
- make predecessor point to new node

how to seach ?
start from the highest level
- go to successor if it is less or equals to key
- go down one level if it is bigger
*/
void put(char* key, char* value, SkipList* skip_list) {  
    SkipListNode** predecessors = calloc(skip_list->max_levels, sizeof(SkipListNode*));
    if(predecessors == NULL) {
        printf("error inserting to skip list");
        exit(1);
    }

    SkipListNode* head = skip_list->head;

    int new_node_level = 0;
    for(int i = 0; i < skip_list->max_levels - 1; i++) {
        int r = rand() % 2;
        if(r == 1) new_node_level++;
        else break;
    }

    int level = skip_list->max_levels - 1;
    SkipListNode* current = head->nexts[level];
    SkipListNode* pre = head;
    while(level >= 0) {
        // the case where there is no node on this level (pre is head)
        // or we reached the end (pre is the node before the end)
        if(current == NULL) {
            predecessors[level] = pre;
            // go back + go down + go forward
            level--;
            if(level >= 0) {
                current = pre->nexts[level];
            }
        }
        else if(strcmp(current->key, key) == 0){
            // found a match change value and return;
            current->value = value;
            free(predecessors);
            return;
        }
        else if(strcmp(current->key, key) < 0){
            // keep going on same level
            pre = current;
            current = pre->nexts[level];
        }
        else {
            // current node key is bigger
            // go back + go down + go forward
            predecessors[level] = pre;
            level--;
            if(level >= 0) {
                current = pre->nexts[level];
            }
        }


    }

    SkipListNode* new_node = createSkipListNode(key, value, skip_list->max_levels);
    for(int i = 0; i <= new_node_level; i++) {
        // make new node point to predecessor next node at each level
        new_node->nexts[i] = predecessors[i]->nexts[i];      
        // make predecessors point to new node
        predecessors[i]->nexts[i] = new_node;
    }
    
    free(predecessors);
    // add size of node and array of pointers
    // size of struct + key + value + nexts array
    skip_list->size = skip_list->size 
        + sizeof(SkipListNode) 
        + strlen(key) + 1
        + strlen(value) + 1
        + sizeof(SkipListNode*) * skip_list->max_levels; 
    printf("size after put %d\n", skip_list->size);
}

// assuming keys are just one char, to make a well structured print
// use padding latter for bigger keys
void print_skip_list(SkipList* skip_list) {
    char** lines = malloc(skip_list->max_levels * sizeof(char*));
    if(lines == NULL) {
        exit(1);
    }

    for(int level = 0; level < skip_list->max_levels; level++) {
        lines[level] = malloc(sizeof(char) * 200);
        if(lines[level] == NULL) {
            exit(1);
        }
    }

    int global_cursor = 0;
    SkipListNode* current = skip_list->head;
    SkipListNode* pre = skip_list->head;
    while(current != NULL) {
        for(int level = 0; level < skip_list->max_levels; level++) {
            int cursor = 0 + global_cursor;
            // a node is only printed on a level if the previeus node point to it
            if(current->key != NULL && (pre->nexts[level] == current || pre == current)) {
                lines[level][cursor] = current->key[0];
            }
            else {
                lines[level][cursor] = ' ';
            }
            cursor++;
            lines[level][cursor] = '-';
            //strcpy(lines[level] + cursor, '-');
            cursor++;
            lines[level][cursor] = '>';
            //strcpy(lines[level] + cursor, '>');
            cursor++;
            
            if(level == skip_list->max_levels - 1) {
                global_cursor = cursor;
            }
        }  
        pre = current;
        current = pre->nexts[0]; // traverse the level 0 as it is the one that has all the nodes
    }

    for(int level = 0; level < skip_list->max_levels; level++) {
        lines[level][global_cursor] = '\0';
    }
    for(int level = skip_list->max_levels - 1; level >= 0 ; level--) {
        printf("%s\n", lines[level]);
        free(lines[level]);
    }
    free(lines);
}

char* get(char* key, SkipList* skip_list) {
    SkipListNode* current = skip_list->head;
    SkipListNode* pre = skip_list->head;

    int level = skip_list->max_levels - 1;
    while(current != NULL && level >= 0) {
        if(current->key == NULL || strcmp(current->key, key) > 0) {
            current = pre;
            level--;
            current = current->nexts[level];
        }
        else if(strcmp(current->key, key) == 0) {
            return current->value;
        }
        else if(strcmp(current->key, key) < 0) {
            pre = current;
            current = current->nexts[level];
        }
    }
    return NULL;
}

void write(SkipList* skip_list) {

}