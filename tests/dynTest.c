#define DYN_IMPLEMENTATION
#include "../dyn.h"
#include <stdio.h>

DynInstantiate(String, char);
DynInstantiate(Floats, f32);
DynInstantiate(Ints, i32);

DynInstantiate(Strings, String);

i32 main(void) {
    String msg;
    StringInit(&msg);
    DynAppendMany(&msg, "Hello, World!\n", 14);

    DynForeach(msg, i)
        printf("%c", msg.items[i]);

    Floats floats;
    FloatsInit(&floats);
    for(inRange(i, 0, 20)) {
        f32 f = i * 3.33;
        DynAppend(&floats, &f);
    }

    DynForeach(floats, i)
        printf("%g%s", floats.items[i], i < (floats.size-1)?", ":"");
    printf("\n");

    Ints ints;
    IntsInit(&ints);
    for(inRange(i, 10, 25))
        DynAppend(&ints, &i);

    DynRemoveUnord(&ints, 10);
    DynRemoveUnord(&ints, 7);
    DynRemoveUnord(&ints, 4);
    DynRemoveUnord(&ints, 2);

    DynForeach(ints, i)
        printf("%d%s", ints.items[i], i < (ints.size-1)?", ":"");
    printf("\n");

    Strings msgs;
    StringsInit(&msgs);
    char *buffer[] = {"Instantiate", "Append", "Reserve", "Resize", "RemoveUnord"};

    String sBuf;
    StringInit(&sBuf);
    for(inRange(i, 0, 5)) {
        DynAppendMany(&sBuf, buffer[i], strlen(buffer[i]));
        DynAppend(&msgs, &sBuf);
        DynResize(&sBuf, 0);
    }

    DynForeach(msgs, i) {
        DynForeach(msgs.items[i], j)
            printf("%c", msgs.items[i].items[j]);
        printf("%s", i < (msgs.size-1)?", ":"");
    }
}