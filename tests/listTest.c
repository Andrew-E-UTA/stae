#include "../list.h"

#include <stdio.h>

ListInstantiate(Ints, i32);

int main (void) {

    Ints ints = IntsCreate();

    ListPop(&ints);             //should do nothing
    ListRemoveAt(&ints, 0);     //should do nothing
    ListRemoveAt(&ints, 100);     //should do nothing

    //pushing
    for(inRange(i, 0, 20)) {
        i32 item = i;
        ListPush(&ints, &item); 
    }

    //popping
    ListPop(&ints);
    ListPop(&ints);
    ListPop(&ints);
    ListPop(&ints);
    ListPop(&ints);
    
    //inserting at an index
    i32 i = 999;
    ListInsertAt(&ints, 0, &i);
    ListInsertAt(&ints, 10, &i);
    ListInsertAt(&ints, ints.count+1, &i);  //doesnt update tail
    
    //removing at an index
    ListRemoveAt(&ints, 0);
    ListRemoveAt(&ints, 9);
    ListRemoveAt(&ints, ints.count);

    //Removing a matched node data
    i=0;
    ListRemoveNode(&ints, &i);
    i=10;
    ListRemoveNode(&ints, &i);
    i=14;
    ListRemoveNode(&ints, &i);  //doesnt update tail

    //iterating
    i=0;
    ListForeach(ints.head, node) {
        printf("Node[%d]: item = %d\n", i++, *((i32*)node->data));
    }
}