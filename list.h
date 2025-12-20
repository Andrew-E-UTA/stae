/*
    Author: Andrew Espinoza
    Contributors:
    License: GPL
    
    Description: A generic Xor based Linked List library, implemented with all 
        purpose nodes and type specific list types to hold the meta data
    Usage:
*/
#ifndef _STAE_LIST_H
#define _STAE_LIST_H
#include "simplify.h"

//Changeable Hooks to C-std lib functions
#ifndef STAE_ASSERT
    #include <assert.h>
    #define STAE_ASSERT assert
#endif

#ifndef STAE_MEMCPY
    #include <string.h>
    #define STAE_MEMCPY memcpy
#endif

#ifndef STAE_CALLOC
    #include <stdlib.h>
    #define STAE_CALLOC calloc
#endif

#ifndef STAE_FREE
    #include <stdlib.h>
    #define STAE_FREE    free
#endif

typedef struct {
    uintptr_t key;
    void* data;
} Node_Generic;


#define ListForeach(s, c)                                                       \
uintptr_t c##key = 0, c##temp;                                                  \
Node_Generic* c = s;                                                            \
for(;c;c##temp=c##key^c->key, c##key=(uintptr_t)c, c=(Node_Generic*)c##temp)

#define ListLast(s, c)          ((s).tail)
#define ListEmpty(s)            ((s).count == 0)
//Needs Get
// #define ListContains(s, item)   (get(&(s), item) != 0)

//TODO: implement deep copy and free 
#define ListInstantiate(Name, Type)                                             \
typedef struct {                                                                \
    Node_Generic* head;                                                         \
    Node_Generic* tail;                                                         \
    size_t count;                                                               \
    size_t data_size;                                                           \
}Name;                                                                          \
                                                                                \
Name Name##Create(void) {                                                       \
    return (Name){                                                              \
        .head = nullptr,                                                        \
        .tail = nullptr,                                                        \
        .count = 0,                                                             \
        .data_size = sizeof(Type)                                               \
    };                                                                          \
}

#define ListPush(p_list, p_item)                                                \
do {                                                                            \
    if(!(p_list))break;                                                         \
    if(!(p_list)->head) {                                                       \
        (p_list)->head = calloc(1, sizeof(Node_Generic));                       \
        (p_list)->head->data = calloc(1, (p_list)->data_size);                  \
        STAE_MEMCPY((p_list)->head->data, p_item, (p_list)->data_size);         \
        break;                                                                  \
    }                                                                           \
    uintptr_t key = 0, t;                                                       \
    Node_Generic* c = (p_list)->head;                                           \
    for(;t=key^c->key;key=(uintptr_t)c, c=(Node_Generic*)t);                    \
    Node_Generic* n = calloc(1, sizeof(Node_Generic));                          \
    n->data = calloc(1, (p_list)->data_size);                                   \
    STAE_MEMCPY(n->data, p_item, (p_list)->data_size);                          \
    c->key ^= (uintptr_t) n;                                                    \
    n->key ^= (uintptr_t) c;                                                    \
    (p_list)->count++;                                                          \
    (p_list)->tail = n;                                                         \
}while(0)

#define ListPop(p_list)                                                         \
do {                                                                            \
    if(!(p_list)) break;                                                        \
    if(ListEmpty(*(p_list))) break;                                             \
    uintptr_t key = 0, t;                                                       \
    Node_Generic* c = (p_list)->head, *last;                                    \
    for(;t=key^c->key;key=(uintptr_t)c, last = c, c=(Node_Generic*)t);          \
    if(c == (p_list)->head) (p_list)->head = nullptr;                           \
    (p_list)->count--;                                                          \
    (p_list)->tail = last;                                                      \
    last->key ^= (uintptr_t)c;                                                  \
    free(c);                                                                    \
}while(0)

#define ListInsertAt(p_list, index, p_item)                                     \
do {                                                                            \
    if(!(p_list)) break;                                                        \
    if((p_list)->count < index - 1 && index) break;                             \
    uintptr_t key = 0, temp;                                                    \
    Node_Generic* c = (p_list)->head, *last;                                    \
    for(size_t j = 0;                                                           \
        c && j != index;                                                        \
        temp=key^c->key, key=(uintptr_t)c, last = c, c=(Node_Generic*)temp, j++);\
    Node_Generic* n = calloc(1, sizeof(Node_Generic));                          \
    n->data = calloc(1, (p_list)->data_size);                                   \
    STAE_MEMCPY(n->data, p_item, (p_list)->data_size);                          \
    if(0 == index) (p_list)->head = n;                                          \
    if((p_list)->count == 0) (p_list)->tail = (p_list)->head;                   \
    if(last){                                                                   \
        last->key ^= (uintptr_t)c;                                              \
        last->key ^= (uintptr_t)n;                                              \
    }                                                                           \
    if(c) {                                                                     \
        c->key ^= (uintptr_t)last;                                              \
        c->key ^= (uintptr_t)n;                                                 \
    }                                                                           \
    (p_list)->count++;                                                          \
}while(0)



#endif//_STAE_LIST_H