#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void db_get(char *key);
void db_set(char *key, char *value);

int main(int argc, char *argv[]) {
    if(argc == 3) {
        if(strcmp(argv[1], "get") == 0) {
            db_get(argv[2]);
        }
    } 
    else if (argc == 4){
        if (strcmp(argv[1], "set") == 0){
            db_set(argv[2], argv[3]);
        }
    }
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