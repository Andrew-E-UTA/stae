/*
    Author: Andrew Espinoza
    Contributors:
    License: GPL
    
    Usage:
        1. DynInstantiate(TypeName, ArrayType)
            a. When appending, only a shallow copy is done, DynInstantate creates a deep copy for the newly instantiated Dyn 
                But other structs would need the user to define their own <Name>Copy function to harness deep copies
            b. A <Name>Free function needs to be provided by the user otherwise no freeing is done 
            c. The create function will use the user defined copy and free functions for the underlying array type
                e.g. <DynName>Create will try to assign TypeCreate to the instance as a function pointer for copy and free
                use needs to either declare them as nullptr or implement them. 
                    - There is CpyFn and FreeFn types to quickly declare null copy and free function handles
        2. All you have to do is just call the library functions on any DynInstantiated type
            a. DynAppend(Dyn* dyn, Type elem)
                - Push to end of array a new element 
            b. DynAppendMany(Dyn* dyn, Type* elems, size_t count)
            c. DynRemoveUnord(Dyn* dyn, size_t index)
            d. DynResize(Dyn* dyn, size_t newSize)
            e. DynReserve(Dyn* dyn, size_t newCapacity)

*/

#ifndef _STAE_DYN_H
#define _STAE_DYN_H

#include "simplify.h"

//Changeable Hooks to C-std lib functions
#include <assert.h>
#define STAE_ASSERT assert

#include <string.h>
#define STAE_MEMCPY memcpy
#define STAE_MEMSET memset

#include <stdlib.h>
#define STAE_REALLOC realloc
#define STAE_FREE    free

typedef void (*CpyFn)(void*, const void*);
typedef void (*FreeFn)(void*);

#define STAE_DYN_INIT_CAP 256
#define DynLast(p_dyn) (p_dyn)->items[STAE_ASSERT((p_dyn)->size), (p_dyn)->size - 1]
#define DynForeach(dyn, index)  for(size_t index = 0; index < (dyn).size; index++)

#define DynReserve(p_dyn, new_capacity)                                         \
do {                                                                            \
    if((p_dyn)->capacity < (new_capacity)) {                                    \
        (p_dyn)->capacity = !(p_dyn)->capacity?                                 \
                            STAE_DYN_INIT_CAP:                                  \
                            (p_dyn)->capacity;                                  \
        while((p_dyn)->capacity < (new_capacity)) (p_dyn)->capacity *= 2;       \
        (p_dyn)->items = STAE_REALLOC((p_dyn)->items,                           \
                         (p_dyn)->capacity * sizeof((p_dyn)->items[0]));        \
    }                                                                           \
} while(0)

#define DynResize(p_dyn, new_size)                                              \
do {                                                                            \
    DynReserve(p_dyn, new_size);                                                \
    (p_dyn)->size = (new_size);                                                 \
} while(0)

#define DynAppend(p_dyn, p_item)                                                \
do {                                                                            \
    DynReserve(p_dyn, (p_dyn)->size + 1);                                       \
    if((p_dyn)->copy) {                                                         \
        if((p_dyn)->create) (p_dyn)->items[(p_dyn)->size] = (p_dyn)->create();  \
        (p_dyn)->copy((p_dyn)->items+(p_dyn)->size++, (p_item));                \
    } else (p_dyn)->items[(p_dyn)->size++] = *(p_item);                         \
} while(0)

#define DynAppendMany(p_dyn, p_items, count)                                    \
do {                                                                            \
    DynReserve(p_dyn, (p_dyn)->size + count);                                   \
    if((p_dyn)->copy)                                                           \
        for(size_t i = 0; i < count; i++)                                       \
            (p_dyn)->copy((p_dyn)->items + ((p_dyn)->size + i), &(p_items)[i]); \
    else STAE_MEMCPY((p_dyn)->items + (p_dyn)->size, p_items, count);           \
    (p_dyn)->size += count;                                                     \
} while(0)

#define DynRemoveUnord(p_dyn, index)                                            \
do {                                                                            \
    STAE_ASSERT((index) < (p_dyn)->size);                                       \
    if((p_dyn)->copy)                                                           \
        (p_dyn)->copy((p_dyn)->items + (index),                                 \
                     &(p_dyn)->items[--(p_dyn)->size]);                         \
    else (p_dyn)->items[index] = (p_dyn)->items[--(p_dyn)->size];               \
} while(0)

#define DynInstantiate(Name, Type)                                              \
typedef struct {                                                                \
    Type *items;                                                                \
    size_t size;                                                                \
    size_t capacity;                                                            \
    CpyFn copy;                                                                 \
    FreeFn free;                                                                \
    Type (*create)();                                                           \
} Name;                                                                         \
                                                                                \
void Name##Copy(Name *dest, Name *src) {                                        \
    DynResize(dest, src->size);                                                 \
    dest->copy = src->copy;                                                     \
    dest->free = src->free;                                                     \
    dest->create = src->create;                                                 \
    if(Type##Copy)                                                              \
        for(size_t i = 0; i < src->size; i++)                                   \
            Type##Copy(dest->items + i, src->items + i);                        \
    else STAE_MEMCPY(dest->items, src->items, src->size * sizeof(Type));        \
}                                                                               \
                                                                                \
void Name##Free(Name *dyn) {                                                    \
    if(Type##Free)                                                              \
        for(size_t i = 0; i < dyn->size; i++)                                   \
            Type##Free(dyn->items + i);                                         \
    STAE_FREE(dyn->items);                                                      \
    STAE_MEMSET(dyn, 0, sizeof(Name));                                          \
}                                                                               \
                                                                                \
Name Name##Create() {                                                           \
    return (Name){                                                              \
        .items=nullptr,                                                         \
        .size=0,                                                                \
        .capacity=0,                                                            \
        .copy= (CpyFn)Type##Copy,                                               \
        .free= (FreeFn)Type##Free,                                              \
        .create= (Type (*)())Type##Create};                                     \
}                                                                               \


//SimpleType handles that will resolve to nullptr for the internal checks
CpyFn u8Copy  = (CpyFn) nullptr;
CpyFn u16Copy = (CpyFn) nullptr;
CpyFn u32Copy = (CpyFn) nullptr;
CpyFn u64Copy = (CpyFn) nullptr;
CpyFn i8Copy  = (CpyFn) nullptr;
CpyFn i16Copy = (CpyFn) nullptr;
CpyFn i32Copy = (CpyFn) nullptr;
CpyFn i64Copy = (CpyFn) nullptr;
CpyFn f32Copy  = (CpyFn) nullptr;
CpyFn f64Copy  = (CpyFn) nullptr;
CpyFn boolCopy = (CpyFn) nullptr;
CpyFn charCopy = (CpyFn) nullptr;

FreeFn u8Free  = (FreeFn) nullptr;
FreeFn u16Free = (FreeFn) nullptr;
FreeFn u32Free = (FreeFn) nullptr;
FreeFn u64Free = (FreeFn) nullptr;
FreeFn i8Free  = (FreeFn) nullptr;
FreeFn i16Free = (FreeFn) nullptr;
FreeFn i32Free = (FreeFn) nullptr;
FreeFn i64Free = (FreeFn) nullptr;
FreeFn f32Free  = (FreeFn) nullptr;
FreeFn f64Free  = (FreeFn) nullptr;
FreeFn boolFree = (FreeFn) nullptr;
FreeFn charFree = (FreeFn) nullptr;

u8 (*u8Create)()        = (u8 (*)()) nullptr;
u16 (*u16Create)()      = (u16 (*)()) nullptr;
u32 (*u32Create)()      = (u32 (*)()) nullptr;
u64 (*u64Create)()      = (u64 (*)()) nullptr;
i8 (*i8Create)()        = (i8 (*)()) nullptr;
i16 (*i16Create)()      = (i16 (*)()) nullptr;
i32 (*i32Create)()      = (i32 (*)()) nullptr;
i64 (*i64Create)()      = (i64 (*)()) nullptr;
f32 (*f32Create)()      = (f32 (*)()) nullptr;
f64 (*f64Create)()      = (f64 (*)()) nullptr;
bool (*boolCreate)()    = (bool (*)()) nullptr;
char (*charCreate)()    = (char (*)()) nullptr;

#endif//_STAE_DYN_H