#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include "skip_list.h"

void write_sstable(SkipList* skip_list);
char* readElement(FILE* file);
void destroy(SkipListNode* head);
char * getWriteFileName();
char* getFileNameForInt(int sstable_number);

SkipList* create_skip_list(int max_level, size_t max_size, size_t max_block_size) {
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
    skip_list->max_block_size = max_block_size;
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
void put(char* k, char* v, SkipList* skip_list) {  
    char* key = strdup(k);
    char* value = strdup(v);
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
            // found a match update size, change value and return;
            skip_list->size = skip_list->size - strlen(current->value) + strlen(value);
            current->value = value;
            free(predecessors);
            printf("size after put %d\n", skip_list->size);
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
    if(skip_list->size >= skip_list->max_size) {
        write_sstable(skip_list);
        // free the skip list nodes
        destroy(skip_list->head);
        // create new skip list head
        SkipListNode* new_head = createSkipListNode(NULL, NULL, skip_list->max_levels);
        skip_list->head = new_head;
        skip_list->size = 0;
    }
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
    while(level >= 0) {
        // current == NULL : we reached the end, go back and down
        if(current == NULL || current->key == NULL || strcmp(current->key, key) > 0) {
            current = pre;
            level--;
            if(level < 0) break;
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

    // scan all the sstables in disk to search for key
    // starting from the most recent sstable
    DIR* dir = opendir(".");
    if(dir == NULL) exit(1);
    struct dirent* entry;
    int file_count = 0;
    while((entry = readdir(dir)) != NULL) {
        if(strncmp("sstable-", entry->d_name, 8) == 0) {
            file_count++;
        }
    }
    file_count--;
    while(file_count >= 0) {
        char* value = getFromDisk(key, skip_list, getFileNameForInt(file_count));
        if(value != NULL) {
            return value;
        }
        file_count--;
    }
    return NULL;
}

char* getFromDisk(char* key, SkipList* skip_list, char* file_name) {
    FILE* file = fopen(file_name, "r");
    if(file == NULL) {
        return NULL; // to pass to other file if they exist
    };

    // get index position
    long index_offset = -1L;
    fseek(file, -8, SEEK_END);
    fread(&index_offset, 8, 1, file);
    // get nbr of elements
    int index_size = -1;
    fseek(file, -12, SEEK_END); // -8 and -4  |index size (4)|offset (8)|
    fread(&index_size, 4, 1, file);
    printf("reading index size %d\n", index_size);
    // load index
    if(index_offset == -1L || index_size == -1) exit(1);
    fseek(file, index_offset, SEEK_SET);
    
    IndexEntry** index = malloc(index_size * sizeof(IndexEntry));
    
    for(int i = 0; i < index_size; i++){
        int key_size = 0;
        fread(&key_size, 4, 1, file);
        char* key_value = malloc(key_size + 1);
        fread(key_value, key_size, 1, file);
        key_value[key_size] = '\0';

        long offset = -1L;
        fread(&offset, 8, 1, file);

        IndexEntry* index_entry = malloc(sizeof(IndexEntry));
        index_entry->key = key_value;
        printf("key value: %s\n", key_value);
        index_entry->offset = offset;

        index[i] = index_entry;
    }

    // binary search to find the key smaller or equal to the search key
    int left  = 0;
    int right = index_size - 1;
    long search_offset = -1L;
    while(left <= right) {
        int mid = (left + right) / 2;
        if(strcmp(index[mid]->key, key) == 0) {
            fseek(file, index[mid]->offset, SEEK_SET);
            readElement(file);// skip the key
            char* value = readElement(file);
            return value;
        }
        else if(strcmp(index[mid]->key, key) > 0) {
            right = mid - 1;
        }
        else {
            // get the last smaller element than the search key
            search_offset = index[mid]->offset;
            left = mid + 1;
        }
    }
    // scan the block to see if the element exist 
    fseek(file, search_offset, SEEK_SET);
    long max = search_offset + skip_list->max_block_size;
    while(search_offset < max) {
        char* key_value = readElement(file);

        if(key_value != NULL && strcmp(key, key_value) == 0) {
            char* value = readElement(file);
            return value;
        }
        search_offset = ftell(file);
    }

    return NULL;
}

// get the key or value size then the content of the key or value and return it
// seek to correct position before using
char* readElement(FILE* file) {
    int size = 0;
    fread(&size, 4, 1, file);
    char* content = malloc(size + 1);
    fread(content, size, 1, file);
    content[size] = '\0';
    return content;
}


// each record is writen as (key size, key data, value size, value data)
// when wrting a record will exceed block max size :
//  - add the firt record key to then index with starting offset
// put the index at the end, and put its size and offset at the verry end (footer) of the file
// structure :
// - data :|key size 4|key value|value size 4|value content|
// - indext|key size 4|key|offset 8|
// at the end we put the size of the index and start offset :
// - |index size (nbr of elements) 4|index offset 8|
void write_sstable(SkipList* skip_list) {
    printf("writing skip list\n");
    if(skip_list->head == NULL || skip_list->head->nexts[0] == NULL) {
        printf("error writing skip list to SSTable");
        exit(1);
    }
    SkipListNode* current = skip_list->head->nexts[0];
    int index_capacity = 16;
    int index_size = 0;
    int index_bytes_size = 0;
    // when index_size == index_capacity double the capacity
    IndexEntry** index_items = malloc(sizeof(IndexEntry*) * index_capacity);

    // use current timestamp or just an encremented value for the name
    char* file_name = getWriteFileName();
    FILE* file_p = fopen(file_name, "ab");
    size_t block_size = 0;
    // first key of the block
    char* first_key = NULL;
    long first_key_offset = -1L; 
    while(current != NULL) {
        long position = ftell(file_p);
        if(position == -1L) {
            printf("error writing skip list to SSTable");
            exit(1);
        }
        int key_size =  strlen(current->key);
        int value_size = strlen(current->value);
        fwrite(&key_size, 4, 1, file_p);
        fwrite(current->key, key_size, 1, file_p);
        fwrite(&value_size, 4, 1, file_p);
        fwrite(current->value, value_size, 1, file_p);

        int current_element_size = 4 + 4 + key_size + value_size;

        // first key of the block and its offset
        if(block_size == 0) {
            first_key = current->key;
            first_key_offset = position;
        }

        // when we reach the block size we add and index entry
        if(block_size == 0) {
            // add key and offset to index
            IndexEntry* index_entry = malloc(sizeof(IndexEntry));
            if(index_entry == NULL) exit(1);
            index_entry->key = first_key;
            index_entry->offset = first_key_offset;
            printf("\n");
            printf("index entry key : %s\n", first_key);
            printf("index entry offset : %d\n", first_key_offset);
            if(index_size == index_capacity) {
                // expande the index array
                IndexEntry** tmp = realloc(index_items, index_capacity * 2 * sizeof(IndexEntry*));
                if(tmp == NULL) exit(1);
                index_items = tmp;
                index_capacity = index_capacity * 2;
                index_items[index_size++] = index_entry;
            }
            else {
                index_items[index_size++] = index_entry;
            }
            index_bytes_size += strlen(first_key) + 8;
        }
        // all elements should be counted in block size not just index element
        block_size += current_element_size;

        // reset first key and block size
        if(block_size >= skip_list->max_block_size) {
            first_key = NULL;
            first_key_offset = -1L;
            block_size = 0;
        }

        current = current->nexts[0];
    }

    // write the index
    long index_position = ftell(file_p);
    printf("writing index position: %d\n", index_position);
    printf("writing index size: %d\n", index_size);
    if(index_position == -1L) {
        printf("error writing skip list to SSTable");
        exit(1);
    }

    int offset_size = 8;
    for(int i = 0; i < index_size; i++) {
        char* key = index_items[i]->key;
        int key_size = strlen(key);
        long offset = index_items[i]->offset;

        fwrite(&key_size, 4, 1, file_p);
        fwrite(key, key_size, 1, file_p);
        fwrite(&offset, offset_size, 1, file_p);
    }
    // write index size and offset at the end
    fwrite(&index_size, 4, 1, file_p);
    fwrite(&index_position, offset_size, 1, file_p);
    fclose(file_p);
    printf("skip list write done\n");
    free(file_name);
}

char * getWriteFileName() {
    DIR* dir = opendir(".");
    if(dir == NULL) exit(1);
    struct dirent* entry = NULL;
    int sstable_count = 0;
    while((entry = readdir(dir)) != NULL) {
        if(strncmp("sstable-", entry->d_name, 8) == 0) {
            sstable_count++;
        }
    }

    return getFileNameForInt(sstable_count);
}

char* getFileNameForInt(int sstable_number) {
    char suffix[15];
    snprintf(suffix, sizeof(suffix), "%d", sstable_number);
    char* new_file_name = malloc(strlen(suffix) + 9 + 4);
    new_file_name[0] = '\0';
    strcat(new_file_name, "sstable-");
    strcat(new_file_name, suffix);
    strcat(new_file_name, ".bin");
    printf("file name %s\n", new_file_name);
    return new_file_name;
}

void destroy(SkipListNode* head) {
    while(head != NULL) {
        SkipListNode* tmp = head->nexts[0];
        free(head->key);
        free(head->value);
        free(head);
        head = tmp;
    }
}