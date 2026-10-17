#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashmap/hashmap.h"

void db_get(char *key, HashMap* hashmap);
void db_set(char *key, char *value, HashMap* hashmap);

int main(int argc, char *argv[]) {
    HashMap* hashmap = create_default_hashmap();
    // put("testjjhjklhjkhjkluuuuuuuuuuuuuuuuufffff", 55L, hashmap);
    // put("test", 65L, hashmap);
    // put("test3", 22L, hashmap);
    // printf("value for test : %d\n", get("test", hashmap));
    // printf("value for test3 : %d\n", get("test3", hashmap));
    // destroy_hashmap(hashmap);
    while(1) {
        printf(">");
        char input[100];
        char* ok = fgets(input, sizeof(input), stdin);
        if(ok == NULL) {
            break;
        }
        input[strcspn(input, "\n")] = '\0';
        
        char* command = strtok(input, " ");
        char* key = strtok(NULL, " ");
        
        if(command == NULL || key == NULL) {
            printf("command and key are required!\n");
            continue;
        }
        if(strcmp(command, "get") == 0){
            printf("parsed = command : %s, key : %s\n", command, key);
            db_get(key, hashmap);
        }
        else if(strcmp(command, "set") == 0) {
            char value[100];
            value[0] = '\0';

            char* first_word_value = strtok(NULL, " ");
            // check for first work in value
            if(first_word_value == NULL) {
                printf("no value passed\n");
                continue;
            }
            strcat(value, first_word_value);
            char* sub_value = strtok(NULL, " ");
            // if there is more words in value
            while(sub_value != NULL){
                strcat(value, " ");
                strcat(value, sub_value);
                sub_value = strtok(NULL, " ");
            }
            printf("parsed = command : %s, key : %s, value : %s\n", command, key, value);
            
            db_set(key, value, hashmap);
            // free(first_word_value);
            // free(sub_value);
        }
        // free(key);
        // free(command);
    }
    destroy_hashmap(hashmap);
}

void db_get(char *key, HashMap* hashmap) {
    FILE *fptr = fopen("db.txt", "r");
    if(fptr == NULL) {
        printf("error");
        exit(1);
    }

    long offset = get(key, hashmap);
    if(offset == -1L) {
        printf("key not found\n");
        fclose(fptr);
        return;
    }
    printf("offset for key is %d\n", offset);
    fseek(fptr, offset, SEEK_SET);
    
    char line[100];
    char* ok = fgets(line, sizeof(line), fptr);
    if(ok == NULL) return;

    strtok(line, ";");
    char* final_value = strtok(NULL, ";");
    printf("value for key %s is %s", key, final_value);// no need for line return already in the value
    fclose(fptr);
}

void db_set(char *key, char *value, HashMap* hashmap) {
    FILE *fptr = fopen("db.txt", "a");
    if(fptr == NULL) {
        printf("error opening file");
        exit(1);
    }
    fseek(fptr, 0, SEEK_END);
    long offset = ftell(fptr);
    if(offset == -1L){
        printf("error inserting to database");
        exit(1);
    }

    printf("setting key %s to offset %d\n", key, offset);
    put(key, offset, hashmap);
    fprintf(fptr, "%s;%s\n", key, value);
    fclose(fptr);
}