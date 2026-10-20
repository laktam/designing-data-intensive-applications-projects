#include <stdlib.h>

typedef struct SkipListNode
{
    char* key;
    char* value;
    struct SkipListNode* nexts[]; // array of pointers to next nodes, one for each level
} SkipListNode;

typedef struct SkipList {
    SkipListNode* head;
    int max_levels;
} SkipList;

SkipList* create_skip_list(int max_level) {
    SkipList* skip_list = malloc(sizeof(*skip_list));
    skip_list->max_levels = max_level;
    // space for head node + spacde for array pointers to max_levels nodes
    SkipListNode* head = malloc(sizeof(SkipListNode) + max_level * sizeof(SkipListNode*));
    head->key = NULL;
    head->value = NULL;
    for(int i = 0; i < max_level; i++) {
        head->nexts[i] = NULL;
    }
    skip_list->head = head;
    return skip_list;
}

void insert(char* key, char* value, SkipList* skip_list) {
    
}