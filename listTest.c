#include "list.h"

#include <stdio.h>

ListInstantiate(Ints, i32);

int main (void) {

    Ints IntList = IntsCreate();
    i32 i = 134;
    ListPush(&IntList, &i);

    return 0;
}