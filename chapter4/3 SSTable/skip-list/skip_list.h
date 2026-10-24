#ifndef SKIPLIST_H
#define SKIPLIST_H

typedef struct SkipListNode
{
    char* key;
    char* value;
    struct SkipListNode* nexts[]; // array of pointers to next nodes, one for each level
} SkipListNode;

typedef struct SkipList {
    SkipListNode* head;
    int max_levels;
    size_t size; // in bytes
    size_t max_size;
} SkipList;

SkipList* create_skip_list(int max_level, size_t max_size);
SkipListNode* createSkipListNode(char* key, char* value, int max_level);
void put(char* key, char* value, SkipList* skip_list);
void print_skip_list(SkipList* skip_list);
char* get(char* key, SkipList* skip_list);

#endif