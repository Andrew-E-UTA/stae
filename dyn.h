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

//TODO: DynCreate have optional args for the free and copy fn

//Changeable Hooks to C-std lib functions
#ifndef STAE_ASSERT
    #include <assert.h>
    #define STAE_ASSERT assert
#endif

#ifndef STAE_MEMCPY
    #include <string.h>
    #define STAE_MEMCPY memcpy
#endif

#ifndef STAE_MEMSET
    #include <string.h>
    #define STAE_MEMSET memset
#endif

#ifndef STAE_REALLOC
    #include <stdlib.h>
    #define STAE_REALLOC realloc
#endif

#ifndef STAE_FREE
    #include <stdlib.h>
    #define STAE_FREE    free
#endif

typedef void (*CpyFn)(void*, const void*);
typedef void (*FreeFn)(void*);
typedef void (*InitFn)(void*);

#define STAE_DYN_INIT_CAP 256
#define DynLast(p_dyn) (p_dyn)->items[STAE_ASSERT((p_dyn)->size > 0), (p_dyn)->size - 1]
#define DynForeach(dyn, index)  for(size_t index = 0; index < (dyn).size; index++)

//======================================================================================
//  Reserve
//======================================================================================


static inline void _GenericDynReserve(void** p_dyn_items, size_t* p_dyn_capacity, 
        size_t item_size, size_t new_capacity) {
    if((*p_dyn_capacity) >= new_capacity) return;
    *p_dyn_capacity = !(*p_dyn_capacity)? STAE_DYN_INIT_CAP: (*p_dyn_capacity);
    while((*p_dyn_capacity) < new_capacity) *p_dyn_capacity *= 2;
    *p_dyn_items = STAE_REALLOC(*p_dyn_items, (*p_dyn_capacity) * item_size);
}

#define DynReserve(p_dyn, new_capacity)                                         \
_GenericDynReserve((void**)&(p_dyn)->items, &(p_dyn)->capacity,                 \
    sizeof((p_dyn)->items[0]), new_capacity)

//======================================================================================
//  Resize
//======================================================================================


static inline void _GenericDynResize(void** p_dyn_items,
        size_t* p_dyn_capacity, size_t* p_dyn_size, FreeFn* p_dyn_free_fn,
        size_t item_size, size_t new_size) {
    if(new_size < (*p_dyn_size)) {
        if(*p_dyn_free_fn)
            for(inRange(i, new_size, *p_dyn_size))
                (*p_dyn_free_fn)((char*)*p_dyn_items + i*item_size);
        //TODO: figure out what to do for primatives that dont need to free
        // else
        //     STAE_ASSERT(0 && "Free Function must be defined to shrink this type");
    } else _GenericDynReserve(p_dyn_items, p_dyn_capacity, item_size, new_size);
    *p_dyn_size = new_size;
}

#define DynResize(p_dyn, new_size)                                              \
_GenericDynResize((void**)&(p_dyn)->items, &(p_dyn)->capacity, &(p_dyn)->size,  \
    &(p_dyn)->free_fn, sizeof((p_dyn)->items[0]), new_size)

//======================================================================================
//  Append
//======================================================================================


static inline void _GenericDynAppend(void** p_dyn_items, 
        size_t* p_dyn_capacity, size_t* p_dyn_size, CpyFn* p_dyn_cpy_fn, 
        InitFn* p_dyn_init_fn, size_t item_size, void* p_item) {
    _GenericDynReserve(p_dyn_items, p_dyn_capacity, item_size, (*p_dyn_size) + 1);
    void* dest = (char*)*p_dyn_items + item_size*(*p_dyn_size);
    if(*p_dyn_cpy_fn) {
        if(*p_dyn_init_fn) (*p_dyn_init_fn)(dest);
        (*p_dyn_cpy_fn)(dest, p_item);
    } else STAE_MEMCPY(dest, p_item, item_size);
    *p_dyn_size = *p_dyn_size + 1;
 }

#define DynAppend(p_dyn, p_item)                                                \
_GenericDynAppend((void**)&(p_dyn)->items, &(p_dyn)->capacity,                  \
    &(p_dyn)->size, &(p_dyn)->copy_fn, &(p_dyn)->init_fn,                       \
    sizeof((p_dyn)->items[0]), p_item);

//======================================================================================
//  AppendMany
//======================================================================================


static inline void _GenericDynAppendMany(void** p_dyn_items, 
        size_t* p_dyn_capacity, size_t* p_dyn_size, CpyFn* p_dyn_cpy_fn, 
        InitFn* p_dyn_init_fn, size_t item_size, void* p_items, size_t items_count) {
    _GenericDynReserve(p_dyn_items, p_dyn_capacity, item_size, (*p_dyn_size) + items_count);
    void* start = (char*)*p_dyn_items + (*p_dyn_size)*item_size;
    if(*p_dyn_cpy_fn)
        for(inRange(i, 0, items_count)) {
            void* dest = (char*)start + i*item_size;
            if(*p_dyn_init_fn) (*p_dyn_init_fn)(dest);
            (*p_dyn_cpy_fn)(dest, (char*)p_items + i*item_size);
        }
    else STAE_MEMCPY((void*)start, p_items, items_count * item_size);
    *p_dyn_size = *p_dyn_size + items_count;
}

#define DynAppendMany(p_dyn, p_items, count)                                    \
_GenericDynAppendMany((void**)&(p_dyn)->items, &(p_dyn)->capacity,              \
    &(p_dyn)->size, &(p_dyn)->copy_fn, &(p_dyn)->init_fn,                       \
    sizeof((p_dyn)->items[0]), p_items, count);

//======================================================================================
//  AppendMany
//======================================================================================

static inline void _GenericDynRemoveUnord(void** p_dyn_items,
        size_t* p_dyn_size, CpyFn* p_dyn_cpy_fn, FreeFn* p_dyn_free_fn, size_t index, size_t item_size) {
    STAE_ASSERT(index < (*p_dyn_size));
    uintptr_t p_item = (uintptr_t)*p_dyn_items + index*item_size;
    uintptr_t p_last = (uintptr_t)*p_dyn_items + (*p_dyn_size - 1)*item_size;
    if(*p_dyn_cpy_fn) (*p_dyn_cpy_fn)((void*)p_item, (void*)p_last);
    else STAE_MEMCPY((void*)p_item, (void*)p_last, item_size);
    if(*p_dyn_free_fn) (*p_dyn_free_fn)((void*)p_item);
    *p_dyn_size = *p_dyn_size - 1;
}

#define DynRemoveUnord(p_dyn, index)                                            \
_GenericDynRemoveUnord((void**)&(p_dyn)->items, &(p_dyn)->size,                 \
    &(p_dyn)->copy_fn, &(p_dyn)->free_fn, index, sizeof((p_dyn)->items[0]));

//======================================================================================
//  Initialization
//======================================================================================


typedef struct {
    CpyFn cpy_fn;
    FreeFn free_fn;
    InitFn init_fn;
} ItemFuncs;

#define DynInit(p_dyn, ...)                                                     \
_GenericInit((void**)&(p_dyn)->items, &(p_dyn)->size, &(p_dyn)->capacity,       \
    &(p_dyn)->copy_fn, &(p_dyn)->free_fn, &(p_dyn)->init_fn,                    \
    (ItemFuncs){.cpy_fn=nullptr, .free_fn=nullptr, .init_fn=nullptr, __VA_ARGS__})

void _GenericInit(void** p_dyn_items, size_t* p_dyn_size, size_t* p_dyn_capacity, 
        CpyFn* p_dyn_cpy_fn, FreeFn* p_dyn_free_fn, InitFn* p_dyn_init_fn,
        ItemFuncs optional) {
    if((!!optional.cpy_fn) != (!!optional.free_fn))
        STAE_ASSERT(0 && "Must have either both or none of the free/copy functions");
    if(optional.init_fn && !optional.cpy_fn)
        STAE_ASSERT(0 && "Initialization function requires copy function");
    *p_dyn_items = nullptr;
    *p_dyn_size = 0;
    *p_dyn_capacity = 0;
    *p_dyn_cpy_fn = optional.cpy_fn;
    *p_dyn_free_fn = optional.free_fn;
    *p_dyn_init_fn = optional.init_fn;
}

//======================================================================================
//  Instantiation
//======================================================================================


#define DynInstantiate(Name, Type, ...)                                         \
typedef struct {                                                                \
    Type *items;                                                                \
    size_t size;                                                                \
    size_t capacity;                                                            \
    CpyFn copy_fn;                                                              \
    FreeFn free_fn;                                                             \
    InitFn init_fn;                                                             \
} Name;                                                                         \
                                                                                \
void Name##Copy(Name *dest, Name *src) {                                        \
    STAE_ASSERT(dest->size == 0);                                               \
    DynResize(dest, src->size);                                                 \
    dest->copy_fn = src->copy_fn;                                               \
    dest->free_fn = src->free_fn;                                               \
    dest->init_fn = src->init_fn;                                               \
    if(dest->copy_fn)                                                           \
        for(size_t i = 0; i < src->size; i++) {                                 \
            if(dest->init_fn) (dest->init_fn)(dest->items + i);                 \
            dest->copy_fn(dest->items + i, src->items + i);                     \
    } else STAE_MEMCPY(dest->items, src->items, src->size * sizeof(Type));      \
}                                                                               \
                                                                                \
void Name##Free(Name *dyn) {                                                    \
    if(dyn->free_fn)                                                            \
        for(size_t i = 0; i < dyn->size; i++)                                   \
            dyn->free_fn(dyn->items + i);                                       \
    STAE_FREE(dyn->items);                                                      \
    STAE_MEMSET(dyn, 0, sizeof(Name));                                          \
}                                                                               \
                                                                                \
void Name##Init(Name* dyn) {                                                    \
    DynInit(dyn);                                                               \
}

#endif//_STAE_DYN_H