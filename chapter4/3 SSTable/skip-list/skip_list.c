#include <stdlib.h>
#include <time.h>
#include <string.h>

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
    // initialize randomness once in skip list creation
    srand(time(NULL));
    SkipList* skip_list = malloc(sizeof(*skip_list));
    skip_list->max_levels = max_level;

    skip_list->head = createSkipListNode(NULL, NULL, max_level);
    return skip_list;
}

SkipListNode* createSkipListNode(char* key, char* value, int max_level) {
    // space for head node + spacde for array pointers to max_levels nodes
    SkipListNode* new_node = malloc(sizeof(SkipListNode) + max_level * sizeof(SkipListNode*));
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
*/
void insert(char* key, char* value, SkipList* skip_list) {  
    SkipListNode** predecessors = malloc(skip_list->max_levels * sizeof(SkipListNode*));
    
    SkipListNode* head = skip_list->head;
    for(int i = 0; i < skip_list->max_levels; i++) {
        SkipListNode* tmp = head->nexts[i];
        SkipListNode* pre = head;
        while(tmp != NULL) {
            if(strcmp(tmp->key, key) == 0) {
                tmp->value = value;
                free(predecessors);
                return;
            }
            else if(strcmp(tmp->key, key)  > 0) {
                break;
            }
            pre = tmp;
            tmp = tmp->nexts[i];
        }
        predecessors[i] = pre;
    }

    SkipListNode* new_node = createSkipListNode(key, value, skip_list->max_levels);
    int new_node_level = 0;
    for(int i = 0; i < skip_list->max_levels; i++) {
        int r = rand() % 2;
        if(r == 1 || i == 0) {
            if(predecessors[i] != NULL) {
                new_node->nexts[i] = predecessors[i]->nexts[i];
            }
            else {
                new_node->nexts[i] = head->nexts[i];
            }
            new_node_level++;
        }
        else {
            break;
        }
    }

    for(int i = 0; i < new_node_level; i++) {
        predecessors[i]->nexts[i] = new_node;
    }
    free(predecessors);
}