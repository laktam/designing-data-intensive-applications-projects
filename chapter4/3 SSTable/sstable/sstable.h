#ifndef SSTABLE_H
#define SSTABLE_H

void write_sstable(SkipList* skip_list);
char* getFromDisk(char* key, SkipList* skip_list, char* file_name);
char* getFileNameForInt(int sstable_number);
void merge(int max_block_size, char* sstable_1, char* sstable_2, char* new_sstable_name);

#endif