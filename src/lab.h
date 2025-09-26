#ifndef LAB_H
#define LAB_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>   // for malloc, free.  
 #include <stdlib.h>   // malloc, free

//alloc/free hook types + externs
typedef void *(*AllocFn)(size_t); //function pointers
typedef void (*FreeFn)(void *); //Function pointers
extern AllocFn lab_alloc_fn; //golbal function pointer for custom allocation
extern FreeFn  lab_free_fn; //golbal function pointer for custom free

/*is a macro wrapper around allocation.
If lab_alloc_fn is set, call it (test hook).
Otherwise call malloc(sz).*/
#ifndef ALLOC
#  define ALLOC(sz)   (lab_alloc_fn ? lab_alloc_fn(sz) : malloc(sz))
#endif
#ifndef DESTROY
#  define DESTROY(p)  do { if (lab_free_fn) lab_free_fn(p); else free(p); } while (0)
#endif

/**
 * @file lab.h
 * @brief Header file for a generic list data structure supporting multiple implementations.
 */
typedef struct List List;

/**
 * @enum ListType
 * @brief Enumeration for selecting the list implementation type.
 */
typedef enum {
    LIST_LINKED_SENTINEL
} ListType;

/**
 * @typedef FreeFunc
 * @brief Function pointer type for freeing elements. If NULL, no action is taken.
 * Must be provided by the user when destroying the list or removing elements.
 *
 */
typedef void (*FreeFunc)(void *);


/**
 * @brief Create a new list of the specified type.
 * @param type The type of list to create (e.g., LIST_LINKED_SENTINEL).
 * @return Pointer to the newly created list, or NULL on failure.
 */
List *list_create(ListType type);

/**
 * @brief Destroy the list and free all associated memory.
 * @param list Pointer to the list to destroy.
 * @param free_func Function to free individual elements. If NULL, elements are not freed.
 */
void list_destroy(List *list, FreeFunc free_func);

/**
 * @brief Append an element to the end of the list.
 * @param list Pointer to the list.
 * @param data Pointer to the data to append.
 * @return true on success, false on failure.
 */
bool list_append(List *list, void *data);

/**
 * @brief Insert an element at a specific index.
 * @param list Pointer to the list.
 * @param index Index at which to insert the element.
 * @param data Pointer to the data to insert.
 * @return true on success, false on failure (e.g., index out of bounds).
 */
bool list_insert(List *list, size_t index, void *data);

/**
 * @brief Remove an element at a specific index.
 * @param list Pointer to the list.
 * @param index Index of the element to remove.
 * @return Pointer to the element, or NULL if index is out of bounds.
 */
void *list_remove(List *list, size_t index);

/**
 * @brief Get a pointer the element at a specific index.
 * @param list Pointer to the list.
 * @param index Index of the element to retrieve.
 * @return Pointer to the element, or NULL if index is out of bounds.
 */
void *list_get(const List *list, size_t index);

/**
 * @brief Get the current size of the list.
 * @param list Pointer to the list.
 * @return The number of elements in the list.
 */
size_t list_size(const List *list);

/**
 * @brief Check if the list is empty.
 * @param list Pointer to the list.
 * @return true if the list is empty, false otherwise.
 */
bool list_is_empty(const List *list);

/**
 * @brief Sorts elements in the list between start and end indices using the provided comparison function.
 * @param list Pointer to the list.
 * @param start The starting index (inclusive).
 * @param end The ending index (exclusive).
 * @param cmp Comparison function: returns <0, 0, >0 for less, equal, greater.
 */
void list_sort(List *list, size_t start, size_t end, int (*cmp)(const void *, const void *));

/**
 * @brief Merges list2 into list1 in sorted order using the provided comparison function.
 * @param list1 Pointer to the first list (destination).
 * @param list2 Pointer to the second list (source, will be emptied).
 * @param cmp Comparison function: returns <0, 0, >0 for less, equal, greater.
 */
void list_merge(List *list1, List *list2, int (*cmp)(const void *, const void *));

/**
 * @brief Comparison function for sorting integers in descending order.
 * @param a Pointer to the first integer.
 * @param b Pointer to the second integer.
 * @return Negative if *a > *b, zero if equal, positive if *a < *b.
 */
int compare_int(const void *a, const void *b);

/**
 * @brief Comparison function for sorting strings in lexicographical order.
 * @param a Pointer to the first string (const char *).
 * @param b Pointer to the second string (const char *).
 * @return Negative if a < b, zero if equal, positive if a > b.
 */
int compare_str(const void *a, const void *b);

/**
 * @brief Checks if the list is sorted according to the provided comparison function.
 * @param list Pointer to the list.
 * @param cmp Comparison function: returns <0, 0, >0 for less, equal, greater.
 * @return true if sorted, false otherwise.
 */
bool is_sorted(const List *list, int (*cmp)(const void *, const void *));
#endif // LAB_H