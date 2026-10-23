#include <stdlib.h>
#include "skip-list/skip_list.h"

int main() {
    SkipList* skip_list = create_skip_list(4);
    insert("b", "b", skip_list);
    insert("a", "b", skip_list);
    insert("d", "b", skip_list);
    insert("g", "b", skip_list);
    insert("g", "b", skip_list);
    insert("k", "b", skip_list);
    insert("e", "b", skip_list);
    insert("t", "b", skip_list);
    insert("y", "b", skip_list);
    insert("u", "b", skip_list);
    insert("i", "b", skip_list);
    insert("o", "b", skip_list);
    insert("p", "b", skip_list);
    insert("m", "b", skip_list);
    insert("l", "b", skip_list);
    print_skip_list(skip_list);
}