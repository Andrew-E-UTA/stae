#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#define nullptr ((void*)0)

#define ListForeach(s, c)\
uintptr_t c##key = 0;\
uintptr_t c##temp;\
Node* c = s;\
for(;c;c##temp=c##key^c->key, c##key=(uintptr_t)c, c=(Node*)c##temp)
    
#define ListLast(s, c)\
uintptr_t c##key = 0;\
uintptr_t c##t;\
Node* c = s;\
for(;c##t=c##key^c->key;c##key=(uintptr_t)c, c=(Node*)c##t);

#define ListEmpty(s) ((s).count == 0)
#define ListContains(s, item) (get(&(s), item) != 0)

typedef struct {
    uintptr_t key;
    int item;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    size_t count;
} List;

void push(List* l, int item) {
    if(!l) return;
    if(!l->head) {
        l->head = calloc(1, sizeof(Node));
        l->head->item = item;
        l->tail = l->head;
        return;
    }
    ListLast(l->head, c);
    
    Node* n = calloc(1, sizeof(Node));
    n->item = item;
    c->key ^= (uintptr_t) n;
    n->key ^= (uintptr_t) c;
    l->count++;
    l->tail = n;
}

void pop(List* l) {
    if(!l) return;
    if(ListEmpty(*l)) return;
    
    uintptr_t key = 0;
    uintptr_t t;
    Node* c = l->head;
    Node* last;
    for(;t=key^c->key;key=(uintptr_t)c, last = c, c=(Node*)t);
    
    if(c == l->head) l->head = nullptr;
    l->count--;
    l->tail = last;

    last->key ^= (uintptr_t)c;
    free(c);
}

void insertAt(List* l, size_t index, int item) {
    if(!l) return;
    if(l->count < index - 1 && index) return;
    
    uintptr_t key = 0;
    uintptr_t temp;
    Node* c = l->head;
    Node* last = 0;
    size_t j = 0;
    for(;c && j != index;temp=key^c->key, key=(uintptr_t)c, last = c, c=(Node*)temp, j++);

    //make new node
    Node* n = calloc(1, sizeof(Node));
    n->key ^= (uintptr_t)last ^ (uintptr_t)c;
    n->item = item;

    if(0 == index) l->head = n;
    if(l->count == 0) l->tail = l->head;
    //update old keys
    if(last){
        last->key ^= (uintptr_t)c;
        last->key ^= (uintptr_t)n;
    }
    if(c) {
        c->key ^= (uintptr_t)last;
        c->key ^= (uintptr_t)n;
    }
    l->count++;
}

void removeAt(List* l, size_t index) {
    if(!l) return;
    if(l->count < index - 1 && index) return;
    
    uintptr_t key = 0;
    uintptr_t temp;
    Node* c = l->head, *last = {0}, *next = {0};
    size_t j = 0;
    for(;c && j != index;temp=key^c->key, key=(uintptr_t)c, last = c, c=(Node*)temp, j++);
    if (!c) return;

    next = (Node*) (c->key ^ key);
    if(0 == index) l->head = next;
    if(l->count + 1 == index) l->tail = last;

    //update old keys
    if(last) {
        last->key ^= (uintptr_t)c;
        last->key ^= (uintptr_t)next;
    }
    if(next) {
        next->key ^= (uintptr_t)c;
        next->key ^= (uintptr_t)last;
    }

    free(c);
    l->count--;
}

void removeNode(List* l, int item) {
    if(!l) return;
    
    uintptr_t key = 0;
    uintptr_t temp;
    Node* c = l->head, *last = {0}, *next = {0};
    size_t j = 0;
    for(;c;temp=key^c->key, key=(uintptr_t)c, last = c, c=(Node*)temp, j++)
        if(c->item == item) break;
    if (!c) return;
    
    next = (Node*) (c->key ^ key);
    if(c == l->head)
        l->head = next;
    //update old keys
    if(last) {
        last->key ^= (uintptr_t)c;
        last->key ^= (uintptr_t)next;
    }
    if(next) {
        next->key ^= (uintptr_t)c;
        next->key ^= (uintptr_t)last;
    }

    free(c);
    l->count--;
}

Node* get(const List *l, size_t index) {
    if(!l) return nullptr;
    bool found;
    size_t j = 0;
    ListForeach(l->head, current) {
        if(j++ == index) {
            found = true;
            break;
        }
    }
    if(found) return current;
    else return nullptr;
}

int main() {
    List l = {0};
    
    push(&l, 1);
    push(&l, 2);
    push(&l, 3);
    push(&l, 4);
    push(&l, 5);

    pop(&l);
    pop(&l);
    pop(&l);
    
    push(&l, 33);
    push(&l, 44);
    push(&l, 55);
    
    insertAt(&l, 0, -1);
    insertAt(&l, 3, 10);
    insertAt(&l, l.count + 1, 99);
    
    removeAt(&l, 0);
    removeAt(&l, 2);
    removeAt(&l, l.count);

    removeNode(&l, 1);
    removeNode(&l, 33);
    removeNode(&l, 55);

    ListForeach(l.head, c)
        printf("Node Data: %d\n", c->item);


    List ints = {0};
    for(size_t i = 0; i < 20; i++)
        push(&ints, (int)i*2);

    printf("Value at index 7: %d\n", get(&ints, 7)->item);

    if(!ListContains(ints, 21))
        printf("Tried getting 21, failed...\n");

    pop(&l);
    pop(&l);
}