#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lab.h"
#include <pthread.h>

#define MIN_STR_LEN 5
#define MAX_STR_LEN 15

/**
 * Generate a random string of length between MIN_STR_LEN and MAX_STR_LEN.
 * Returns a heap-allocated string. Caller must free the result.
 * AI Use: AI Assisted
 */
char *random_string() {
    int len = MIN_STR_LEN + rand() % (MAX_STR_LEN - MIN_STR_LEN + 1);
    char *str = malloc((size_t)len + 1);
    for (int i = 0; i < len; ++i)
        str[i] = (char)('a' + rand() % 26);
    str[len] = '\0';
    return str;
}

/**
 * Struct for passing arguments to sorting threads.
 * AI Use: AI Assisted
 */
typedef struct {
    List *list;
    size_t start;
    size_t end;   // half-open [start, end)
    int (*cmp)(const void *, const void *);
} SortArgs;

static void *sort_thread(void *arg) {
    SortArgs *a = (SortArgs *)arg;
    list_sort(a->list, a->start, a->end, a->cmp);
    return NULL;
}

#ifndef TEST 
/**
 * Main entry point for the threaded list sort/merge program.
 * Handles command-line arguments, list creation, population, threaded sorting, merging, and output.
 * Usage: myapp <int|string> <length>
 * AI Use: AI Assisted
 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <int|string> <length>\n", argv[0]);
        return 1;
    }

    srand((unsigned)time(NULL));
    int length = atoi(argv[2]);
    if (length <= 0) {
        printf("Length must be a positive integer.\n");
        return 1;
    }

      const int use_ints = (strcmp(argv[1], "int") == 0);
    const int use_strs = (strcmp(argv[1], "string") == 0);
    if (!use_ints && !use_strs) {
        fprintf(stderr, "Unknown data type: %s (expected 'int' or 'string')\n", argv[1]);
        return 1;
    }

    char *endp = NULL;
    long nlong = strtol(argv[2], &endp, 10);
    if (*argv[2] == '\0' || *endp != '\0' || nlong <= 0) {
        fprintf(stderr, "Length must be a positive integer.\n");
        return 1;
    }
    size_t n = (size_t)nlong;

    List *list = list_create(LIST_LINKED_SENTINEL);
    if (!list) { fprintf(stderr, "Failed to create list.\n"); return 1; }

    // Populate
    if (use_ints) {
        for (size_t i = 0; i < n; ++i) {
            int *p = (int *)malloc(sizeof *p);
            if (!p) { perror("malloc"); return 1; }
            *p = rand() % 1000;
            if (!list_append(list, p)) { fprintf(stderr, "append failed\n"); return 1; }
        }
    } else { // strings
        for (size_t i = 0; i < n; ++i) {
            char *s = random_string();
            if (!list_append(list, s)) { fprintf(stderr, "append failed\n"); return 1; }
        }
    }

    /**
     * Thread Function--two thread sort on one list: [0, mid) and [mid, n)
     * AI Use: AI Assisted
     */
    size_t mid = n / 2;
    int (*cmp)(const void *, const void *) = use_ints ? compare_int : compare_str;

    pthread_t t1, t2;
    SortArgs a1 = { list, 0,   mid, cmp };
    SortArgs a2 = { list, mid, n,   cmp };

    if (pthread_create(&t1, NULL, sort_thread, &a1) != 0) { perror("pthread_create"); return 1; }
    if (pthread_create(&t2, NULL, sort_thread, &a2) != 0) { perror("pthread_create"); return 1; }
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Merge the two sorted sublists into a single sorted list.
    // Keep to the "one list for threads" rule. For merging,move
    // the second half to a temporary list and use your list_merge().
    List *right = list_create(LIST_LINKED_SENTINEL);
    if (!right) { fprintf(stderr, "Failed to create temp list.\n"); return 1; }

    // Move elements [mid, n) from 'list' into 'right' preserving order.
    for (size_t k = mid; k < n; ++k) {
        void *elem = list_remove(list, mid);  // always remove at current mid
        (void)list_append(right, elem);
    }

    // Merge right into list using your compare function
    list_merge(list, right, cmp);

    // right should now be empty
    if (list_size(right) != 0) {
        fprintf(stderr, "WARNING: expected temp list to be empty after merge.\n");
    }
    list_destroy(right, NULL);

    // Verify final sort
    if (!is_sorted(list, cmp)) {
        fprintf(stderr, "ERROR: final list not sorted\n");
        // continue to print for debugging
    }

    // Output and free payloads
    if (use_ints) {
        printf("Sorted integers:\n");
        for (size_t i = 0; i < list_size(list); ++i) {
            int *p = (int *)list_get(list, i);
            printf("%d%s", *p, (i + 1 == list_size(list)) ? "\n" : " ");
            free(p);
        }
    } else {
        printf("Sorted strings:\n");
        for (size_t i = 0; i < list_size(list); ++i) {
            char *s = (char *)list_get(list, i);
            printf("%s\n", s);
            free(s);
        }
    }

    list_destroy(list, NULL);
    return 0;
}
#endif
