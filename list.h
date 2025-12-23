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

#ifndef STAE_MEMCMP
    #include <string.h>
    #define STAE_MEMCMP memcmp
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

//======================================================================================
//  Push
//======================================================================================
/*
*/
static inline void _GenericListPush(Node_Generic** p_list_head, Node_Generic** p_list_tail, size_t *p_list_count,
         void* p_item, size_t item_size) {
    Node_Generic* p_head = *p_list_head;
    if(!p_head) {
        p_head = calloc(1, sizeof(Node_Generic));
        p_head->data = calloc(1, item_size);
        STAE_MEMCPY(p_head->data, p_item, item_size);
        *p_list_head = p_head;
        *p_list_tail = *p_list_head;
        *p_list_count = *p_list_count + 1;
        return;
    }
    uintptr_t key = 0, temp_key;
    Node_Generic* p_curr_node = p_head;
    for(;temp_key=key^p_curr_node->key;key=(uintptr_t)p_curr_node, p_curr_node=(Node_Generic*)temp_key);
    Node_Generic* n = calloc(1, sizeof(Node_Generic));
    n->data = calloc(1, item_size);
    STAE_MEMCPY(n->data, p_item, item_size);
    p_curr_node->key ^= (uintptr_t) n;
    n->key ^= (uintptr_t) p_curr_node;
    *p_list_count = *p_list_count + 1;
    *p_list_tail = n;
}

#define ListPush(p_list, p_item)                                                \
_GenericListPush(&((p_list)->head), &((p_list)->tail), &((p_list)->count),      \
    p_item, (p_list)->data_size)

//======================================================================================
//  Pop
//======================================================================================
/*
*/
static inline void _GenericListPop(Node_Generic** p_list_head, Node_Generic** p_list_tail, size_t* p_list_count) {
    if(*p_list_count == 0) return;
    uintptr_t key = 0, temp_key;
    Node_Generic* p_curr_node = *p_list_head, *p_last_node;
    for(;temp_key=key^p_curr_node->key;
        key=(uintptr_t)p_curr_node, p_last_node = p_curr_node, p_curr_node=(Node_Generic*)temp_key);
    if(p_curr_node == *p_list_head) *p_list_head = nullptr;
    *p_list_count = *p_list_count - 1;
    *p_list_tail = p_last_node;
    p_last_node->key ^= (uintptr_t)p_curr_node;
    free(p_curr_node);
}

#define ListPop(p_list)                                                         \
    _GenericListPop(&((p_list)->head), &((p_list)->tail), &((p_list)->count))

//======================================================================================
//  InsertAt
//======================================================================================
/*
*/
static inline void _GenericListInsertAt(Node_Generic** p_list_head, Node_Generic** p_list_tail, size_t* p_list_count,
        void* p_item, size_t item_size, size_t index) {
    if(*p_list_count < index - 1 && index) return;
    uintptr_t key = 0, temp_key;
    Node_Generic* p_curr_node = *p_list_head, *p_last_node = 0;
    for(size_t j = 0; p_curr_node && j != index;
        temp_key = key ^ (p_curr_node->key),
        key = (uintptr_t)p_curr_node,
        p_last_node = p_curr_node,
        p_curr_node = (Node_Generic*)temp_key,
        j++);
    Node_Generic* p_new_node = malloc(sizeof(Node_Generic));
    p_new_node->key = (uintptr_t)p_last_node ^ (uintptr_t)p_curr_node;
    p_new_node->data = malloc(item_size);
    STAE_MEMCPY(p_new_node->data, p_item, item_size);
    if(0 == index)  *p_list_head = p_new_node;
    if(0 == *p_list_count) 
        *p_list_tail =  *p_list_head;
    if(index == *p_list_count + 1) 
        *p_list_tail = p_new_node;
    if(p_last_node){
        p_last_node->key ^= (uintptr_t)p_curr_node;
        p_last_node->key ^= (uintptr_t)p_new_node;
    }
    if(p_curr_node) {
        p_curr_node->key ^= (uintptr_t)p_last_node;
        p_curr_node->key ^= (uintptr_t)p_new_node;
    }
    *p_list_count = *p_list_count + 1;
}

#define ListInsertAt(p_list, index, p_item)                                     \
    _GenericListInsertAt(&((p_list)->head), &((p_list)->tail), &((p_list)->count),  \
        p_item, (p_list)->data_size, index)

//======================================================================================
//  RemoveAt
//======================================================================================
/*
*/
static inline void _GenericListInsertAfter() {

}

#define ListInsertAfter(p_list, p_ref_item, p_item)                                 \
    _GenericListInsertAfter(&((p_list)->head), &((p_list)->tail), &((p_list)->count), p_ref_item, p_item, (p_list)->data_size,)

//======================================================================================
//  RemoveAt
//======================================================================================
/*
*/
static inline void _GenericListRemoveAt(Node_Generic** p_list_head, Node_Generic** p_list_tail, size_t* p_list_count, 
        size_t index) {
    if(*p_list_count < index - 1 && index) return;
    uintptr_t key = 0, temp_key;
    Node_Generic* p_curr_node = *p_list_head, *p_last_node = {0}, *p_next_node = {0};
    size_t j = 0;
    for(;p_curr_node && j != index;
        temp_key=key^p_curr_node->key,
        key=(uintptr_t)p_curr_node,
        p_last_node = p_curr_node,
        p_curr_node=(Node_Generic*)temp_key,
        j++);
    if (!p_curr_node) return;
    p_next_node = (Node_Generic*) (p_curr_node->key ^ key);
    if(0 == index) *p_list_head = p_next_node;
    if(*p_list_count == index) *p_list_tail = p_last_node;
    if(p_last_node) {
        p_last_node->key ^= (uintptr_t)p_curr_node;
        p_last_node->key ^= (uintptr_t)p_next_node;
    }
    if(p_next_node) {
        p_next_node->key ^= (uintptr_t)p_curr_node;
        p_next_node->key ^= (uintptr_t)p_last_node;
    }
    free(p_curr_node);
    *p_list_count = *p_list_count - 1;
}

#define ListRemoveAt(p_list, index)                                             \
    _GenericListRemoveAt(&((p_list)->head), &((p_list)->tail), &((p_list)->count), index)

//======================================================================================
//  RemoveAt
//======================================================================================
/*
*/
static inline void _GenericListRemoveNode(Node_Generic** p_list_head, Node_Generic** p_list_tail, size_t* p_list_count, 
        void* p_item, size_t item_size) {
    uintptr_t key = 0, temp_key;
    Node_Generic* p_curr_node = *p_list_head, *p_last_node = {0}, *p_next_node = {0};
    size_t j = 0;
    for(;p_curr_node;
        temp_key=key^p_curr_node->key,
        key=(uintptr_t)p_curr_node,
        p_last_node = p_curr_node,
        p_curr_node=(Node_Generic*)temp_key,
        j++){
        if(0 == STAE_MEMCMP(p_curr_node->data, p_item, item_size)) break;
    }
    if (!p_curr_node) return;
    p_next_node = (Node_Generic*) (p_curr_node->key ^ key);
    if(p_curr_node == *p_list_head)
        *p_list_head = p_next_node;
    if(j == *p_list_count)
        *p_list_tail = p_last_node;
    //update old keys
    if(p_last_node) {
        p_last_node->key ^= (uintptr_t)p_curr_node;
        p_last_node->key ^= (uintptr_t)p_next_node;
    }
    if(p_next_node) {
        p_next_node->key ^= (uintptr_t)p_curr_node;
        p_next_node->key ^= (uintptr_t)p_last_node;
    }
    free(p_curr_node);
    *p_list_count = *p_list_count - 1;
}

#define ListRemoveNode(p_list, p_item)                                          \
    _GenericListRemoveNode(&((p_list)->head), &((p_list)->tail), &((p_list)->count),      \
    p_item, (p_list)->data_size)

//======================================================================================
//  GetNodeByIndex
//======================================================================================
/*
*/
static inline void* _GenericListGetNode(Node_Generic** p_list_head, size_t index) {
    bool found;
    size_t j = 0;
    ListForeach(*p_list_head, p_curr_node)
        if(j++ == index) return p_curr_node;
    return nullptr;
}

#define ListGetNodeByIndex(p_list, index)                                       \
    _GenericListGetNode(&((p_list)->head), index)

#endif//_STAE_LIST_H