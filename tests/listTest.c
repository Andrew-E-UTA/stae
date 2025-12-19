#include "../list.h"

#include <stdio.h>

ListIntantiate(Ints, i32);

int main (void) {

    Ints i32List = IntsCreate();
    for(inRange(i, 0, 20)) {
        i32 item = i*2;
        ListPush(&i32List, &item); 
    }

    ListPop(&i32List);
    ListPop(&i32List);
    ListPop(&i32List);
    ListPop(&i32List);
    ListPop(&i32List);

    i32 i = 0;
    ListForeach(i32List.head, node) {
        printf("Node[%d]: item = %d\n", i++, *((i32*)node->data));
    }

}