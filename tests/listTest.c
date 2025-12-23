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
    ListInsertAt(&ints, ints.count+1, &i);  
    
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
    ListRemoveNode(&ints, &i);

    //iterating
    i=0;
    ListForeach(ints.head, node) {
        printf("Node[%d]: item = %d\n", i++, *((i32*)node->data));
    }

    //iterate by index
    for(size_t k = 0; k < ints.count; k++)
        printf("Node[%d]: item = %d\n", k, *((i32*)((Node_Generic*)ListGetNodeByIndex(&ints, k))->data));
}