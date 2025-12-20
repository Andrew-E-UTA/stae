#include "../list.h"

#include <stdio.h>

ListInstantiate(Ints, i32);

int main (void) {

    Ints ints = IntsCreate();
    for(inRange(i, 0, 20)) {
        i32 item = i;
        ListPush(&ints, &item); 
    }

    ListPop(&ints);
    ListPop(&ints);
    ListPop(&ints);
    ListPop(&ints);
    ListPop(&ints);

    i32 i = 999;
    ListInsertAt(&ints, 0, &i);
    ListInsertAt(&ints, 10, &i);
    ListInsertAt(&ints, ints.count+1, &i);
    
    ListRemoveAt(&ints, 0);
    ListRemoveAt(&ints, 9);
    ListRemoveAt(&ints, ints.count);

    i=0;
    ListForeach(ints.head, node) {
        printf("Node[%d]: item = %d\n", i++, *((i32*)node->data));
    }


}