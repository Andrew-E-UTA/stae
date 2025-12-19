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

typedef struct _node {
    void* xor;
    void* data;
}Node;

#define ListPush(p_list, p_item)                                                \
do {                                                                            \
    
}while(0)


#define ListInstantiate(Name, Type)                                             \
typedef struct {                                                                \
    Node* head;                                                                 \
    Node* tail;                                                                 \
    size_t typeSize;                                                            \
}Name;                                                                          \
                                                                                \
Name Name##Create(void) {                                                       \
    return (Name){.head=nullptr, .tail=nullptr, .typeSize=sizeof(Type)};        \
}



#endif//_STAE_LIST_H