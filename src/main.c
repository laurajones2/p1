#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lab.h"

#define MIN_STR_LEN 5
#define MAX_STR_LEN 15

// Generate a random string of length between MIN_STR_LEN and MAX_STR_LEN
char *random_string() {
    int len = MIN_STR_LEN + rand() % (MAX_STR_LEN - MIN_STR_LEN + 1);
    char *str = malloc(len + 1);
    for (int i = 0; i < len; ++i)
        str[i] = 'a' + rand() % 26;
    str[len] = '\0';
    return str;
}

#ifndef TEST 
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

    List *list = list_create(LIST_LINKED_SENTINEL);
    if (!list) {
        printf("Failed to create list.\n");
        return 1;
    }

    if (strcmp(argv[1], "int") == 0) {
        for (int i = 0; i < length; ++i) {
            int *val = malloc(sizeof(int));
            *val = rand() % 1000;
            list_append(list, val);
        }
        list_sort(list, 0, list_size(list), compare_int);
        printf("Sorted integers:\n");
        for (size_t i = 0; i < list_size(list); ++i) {
            int *val = (int *)list_get(list, i);
            printf("%d ", *val);
            free(val);
        }
        printf("\n");
    } else if (strcmp(argv[1], "string") == 0) {
        for (int i = 0; i < length; ++i) {
            char *str = random_string();
            list_append(list, str);
        }
        list_sort(list, 0, list_size(list), compare_str);
        printf("Sorted strings:\n");
        for (size_t i = 0; i < list_size(list); ++i) {
            char *str = (char *)list_get(list, i);
            printf("%s\n", str);
            free(str);
        }
    } else {
        printf("Unknown data type: %s\n", argv[1]);
        list_destroy(list, NULL);
        return 1;
    }

    list_destroy(list, NULL);
    return 0;
}
#endif