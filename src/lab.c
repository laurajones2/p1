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

/**
 * Sorts the elements in the list between start and end indices using the provided comparison function.
 * The comparison function should return <0, 0, >0 for less, equal, greater.
 * AI Use: AI assisted
 */
void list_sort(List *list, size_t start, size_t end,
               int (*cmp)(const void *, const void *)) {
    if (!list || !list->sentinel || !cmp) return;
    if (start >= end || end > list->size) return;

    size_t count = end - start;
    if (count < 2) return;

    // Move to the first node in the subrange
    Node *node_i = list->sentinel->next;
    for (size_t s = 0; s < start; ++s) {
    node_i = node_i->next; // GCOVR_EXCL_LINE
    // GCOVR_EXCL_START
    if (node_i == list->sentinel) return; // out-of-bounds guard
    // GCOVR_EXCL_STOP
}


    // Simple selection-style sort over [start, end)
    for (size_t i = 0; i < count - 1; ++i) {
        Node *min_node = node_i;

        // Walk the remainder of the subrange to find the minimal element
        Node *node_j = node_i->next;
        for (size_t j = i + 1; j < count; ++j) {
            if (node_j == list->sentinel) break; // safety with circular list
            if (cmp(min_node->data, node_j->data) > 0) {
                min_node = node_j;
            }
            node_j = node_j->next;
        }

        // Swap payload pointers if a smaller element was found
        if (min_node != node_i) {
            void *tmp = node_i->data;
            node_i->data = min_node->data;
            min_node->data = tmp;
        }

        // Advance to next position in the subrange
        node_i = node_i->next;
        if (node_i == list->sentinel) break; // safety
    }
}


/**
 * Merges list2 into list1 in sorted order using the provided comparison function.
 * The merged result is stored in list1. list2 will be emptied but not destroyed.
 * AI Use: Written By AI
 */
void list_merge(List *list1, List *list2, int (*cmp)(const void *, const void *)) {
    if (!list1 || !list2 || !cmp || !list1->sentinel || !list2->sentinel) return;

    Node *curr2 = list2->sentinel->next;
    while (curr2 != list2->sentinel) {
        void *data = curr2->data;
        // Find insertion point in list1
        Node *curr1 = list1->sentinel->next;
        size_t index = 0;
        while (curr1 != list1->sentinel && cmp(data, curr1->data) > 0) {
            curr1 = curr1->next;
            index++;
        }
        list_insert(list1, index, data);
        curr2 = curr2->next;
    }
    // Empty list2
    Node *sentinel2 = list2->sentinel;
    Node *curr = sentinel2->next;
    while (curr != sentinel2) {
        Node *next = curr->next;
        DESTROY(curr);
        curr = next;
    }
    sentinel2->next = sentinel2;
    sentinel2->prev = sentinel2;
    list2->size = 0;
}

/**
 * Comparison function for sorting integers in descending order.
 * AI Use: Written By AI
 */
int compare_int(const void *a, const void *b) {
    int int_a = *(const int *)a;
    int int_b = *(const int *)b;
    return int_b - int_a;
}

#include <string.h>

/**
 * Comparison function for sorting strings in lexicographical order.
 * AI Use: Written By AI
 */
int compare_str(const void *a, const void *b) {
    const char *str_a = *(const char **)a;
    const char *str_b = *(const char **)b;
    return strcmp(str_a, str_b);
}

/**
 * Checks if the list is sorted according to the provided comparison function.
 * AI Use: Written By AI
 */
bool is_sorted(const List *list, int (*cmp)(const void *, const void *)) {
    if (!list || !cmp || list->size < 2) return true;
    Node *curr = list->sentinel->next;
    for (size_t i = 1; i < list->size; ++i) {
        Node *next = curr->next;
        if (cmp(curr->data, next->data) > 0) {
            return false;
        }
        curr = next;
    }
    return true;
}