#include "lab.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef ALLOC
#define ALLOC(size) malloc(size)
#endif
#ifndef DESTROY
#define DESTROY(ptr) free(ptr)
#endif

/**
 * Global function pointer for custom allocation/deallocation. Set to NULL to use default ALLOC.
 * AI Use: Written By AI
 */
AllocFn lab_alloc_fn = NULL;
FreeFn  lab_free_fn  = NULL;

/**
 * Node structure for the circular, doubly linked list.
 * AI Use: AI Assisted
 */
typedef struct Node {
    void *data;
    struct Node *prev;
    struct Node *next;
} Node;

/**
 * List structure definition for the circular, doubly linked list with sentinel node.
 * AI Use: AI Assisted
 */
struct List {
    size_t size;
    ListType type;
    Node *sentinel;
};

/**
 * Creates a new circular, doubly linked list with a sentinel node.
 * AI Use: AI Assisted
 */
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

/**
 * Destroys the list and frees all associated memory. Calls free_func on each data element if provided.
 * AI Use: AI Assisted
 */
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

/**
 * Appends a new element to the end of the list.
 * AI Use: AI Assisted
 */
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

/**
 * Inserts a new element at the specified index in the list.
 * AI Use: AI Assisted
 */
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

/**
 * Removes the element at the specified index from the list and returns its data pointer.
 * AI Use: AI Assisted
 */
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

/**
 * Returns the data pointer at the specified index in the list.
 * AI Use: AI Assisted
 */
void *list_get(const List *list, size_t index) {
    if (!list || !list->sentinel) return NULL;
    if (index >= list->size) return NULL;
    Node *curr = list->sentinel->next;
    for (size_t i = 0; i < index; ++i) {
        curr = curr->next;
    }
    return curr->data;
}

/**
 * Returns the number of elements in the list.
 * AI Use: AI Assisted
 */
size_t list_size(const List *list) {
    if (!list) return 0;
    return list->size;
}

/**
 * Returns true if the list is empty, false otherwise.
 * AI Use: AI Assisted
 */
bool list_is_empty(const List *list) {
    if (!list) return true;
    return list->size == 0;
}
