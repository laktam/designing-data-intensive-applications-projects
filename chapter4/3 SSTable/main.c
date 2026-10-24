#include <stdlib.h>
#include <stdio.h>
#include "skip-list/skip_list.h"

int main() {
    SkipList* skip_list = create_skip_list(4, 1024);
    put("b", "b", skip_list);
    put("a", "a", skip_list);
    put("d", "d", skip_list);
    put("g", "g", skip_list);
    put("g", "g", skip_list);
    put("k", "k", skip_list);
    put("e", "e", skip_list);
    put("t", "t", skip_list);
    put("y", "y", skip_list);
    put("u", "u", skip_list);
    put("i", "i", skip_list);
    put("o", "b", skip_list);
    put("p", "p", skip_list);
    put("m", "m", skip_list);
    put("g", "test value", skip_list);
    print_skip_list(skip_list);

    printf("%s", get("g", skip_list));
}