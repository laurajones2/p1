#include "lab.h"

#include <stdio.h>
#include <stdlib.h>

#ifndef ALLOC
#define ALLOC(size) malloc(size)
#endif
#ifndef DESTROY
#define DESTROY(ptr) free(ptr)
#endif

AllocFn lab_alloc_fn = NULL;
FreeFn  lab_free_fn  = NULL;


//Create List
// private node structure
typedef struct Node {
    void *data;
    struct Node *prev;
    struct Node *next;
} Node;

// list structure definition
struct List {
    size_t size;
    ListType type;
    Node *sentinel;
};

List *list_create(ListType type) {
    // Allocate memory for the list
    List *list = ALLOC(sizeof(List));
    if (list == NULL) {
        return NULL; // allocation failed
    }

    // Allocate memory for the sentinel node
    Node *sentinel = ALLOC(sizeof(Node));
    if (sentinel == NULL) {
    DESTROY(list);
        return NULL;
    }

    // Initialize the sentinel node (circular self-links)
    sentinel->data = NULL;
    sentinel->next = sentinel;
    sentinel->prev = sentinel;

    // Initialize the list
    list->size = 0;
    list->type = type;
    list->sentinel = sentinel;

    return list;
}

//Detroy List

void list_destroy(List *list, FreeFunc free_func) {
    if (!list) return;
    Node *sentinel = list->sentinel;
    Node *curr = sentinel->next;
    while (curr != sentinel) {
        Node *next = curr->next;
        if (free_func && curr->data) {
            free_func(curr->data);
        }
        DESTROY(curr);
        curr = next;
    }
    DESTROY(sentinel);
    DESTROY(list);
}

//Append List
bool list_append(List *list, void *data) {
    if (!list || !list->sentinel) return false;
    Node *new_node = ALLOC(sizeof(Node));
    if (!new_node) return false;
    new_node->data = data;

    Node *sentinel = list->sentinel;
    Node *last = sentinel->prev;

    // Insert new_node before sentinel
    new_node->next = sentinel;
    new_node->prev = last;
    last->next = new_node;
    sentinel->prev = new_node;

    list->size++;
    return true;
}

//Insert List
bool list_insert(List *list, size_t index, void *data) {
    if (!list || !list->sentinel) return false;
    if (index > list->size) return false; // index out of bounds

    Node *sentinel = list->sentinel;
    Node *curr = sentinel->next;
    for (size_t i = 0; i < index; ++i) {
        curr = curr->next;
    }

    Node *new_node = ALLOC(sizeof(Node));
    if (!new_node) return false;
    new_node->data = data;

    // Insert new_node before curr
    new_node->prev = curr->prev;
    new_node->next = curr;
    curr->prev->next = new_node;
    curr->prev = new_node;

    list->size++;
    return true;
}

//Remove List 

void *list_remove(List *list, size_t index) {
    if (!list || !list->sentinel) return NULL;
    if (index >= list->size) return NULL;
    Node *sentinel = list->sentinel;
    Node *curr = sentinel->next;
    for (size_t i = 0; i < index; ++i) {
        curr = curr->next;
    }
    void *data = curr->data;
    curr->prev->next = curr->next;
    curr->next->prev = curr->prev;
    DESTROY(curr);
    list->size--;
    return data;
}

//List get

void *list_get(const List *list, size_t index) {
    if (!list || !list->sentinel) return NULL;
    if (index >= list->size) return NULL;
    Node *curr = list->sentinel->next;
    for (size_t i = 0; i < index; ++i) {
        curr = curr->next;
    }
    return curr->data;
}

//Size List

size_t list_size(const List *list) {
    if (!list) return 0;
    return list->size;
}

//Empty List?

bool list_is_empty(const List *list) {
    if (!list) return true;
    return list->size == 0;
}
