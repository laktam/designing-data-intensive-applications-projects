#include <stdio.h>
#include <string.h>
#include "skip-list/skip_list.h"

int main() {
    SkipList* skip_list = create_skip_list(4, 60, 25);
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
            char* result = get(key, skip_list);
            printf("%s\n", result);
        }
        else if(strcmp(command, "print") == 0){
            print_skip_list(skip_list);
        }
        else if(strcmp(command, "set") == 0) {
            char value[100];
            value[0] = '\0';

            char* first_word_value = strtok(NULL, " ");
            // check for first work in value
            if(first_word_value == NULL) {
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
            
            put(key, value, skip_list);
        }
    }

    //destroy skip list
}