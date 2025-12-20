#include "../list.h"

#include <stdio.h>

ListInstantiate(Ints, i32);
ListInstantiate(Floats, f32);

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
    {    
    i32 i = 0;
    ListForeach(i32List.head, node) {
        printf("Node[%d]: item = %d\n", i++, *((i32*)node->data));
    }
    }
    Floats f32List = FloatsCreate();
    for(inRange(i, 0, 20)) {
        f32 item = i*1.3;
        ListPush(&f32List, &item); 
    }

    i32 i = 0;
    ListForeach(f32List.head, node) {
        printf("Node[%d]: item = %g\n", i++, *((f32*)node->data));
    }
}