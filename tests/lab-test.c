
#include <stdlib.h>
#include <stdio.h>
#include "harness/unity.h"



// --- ALLOC/DESTROY mocking for allocation failure testing ---
static int alloc_fail_after = -1;
static int alloc_call_count = 0;
void *test_alloc(size_t size) {
    alloc_call_count++;
    if (alloc_fail_after > 0 && alloc_call_count == alloc_fail_after) {
        return NULL;
    }
    return malloc(size);
}
void test_destroy(void *ptr) {
    free(ptr);
}


#include "../src/lab.h"

void setUp(void) {
   alloc_fail_after = -1;
    alloc_call_count = 0;

     // Redirect allocations in lab.c to our test hooks
    lab_alloc_fn = test_alloc;
    lab_free_fn  = test_destroy;
}
void tearDown(void) {}

static void test_create_and_destroy(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  TEST_ASSERT_NOT_NULL(list);
  list_destroy(list, NULL);
}

static void test_alloc_hook_smoke(void) {
    alloc_fail_after = -1;
    alloc_call_count = 0;

    List *list = list_create(LIST_LINKED_SENTINEL);
    TEST_ASSERT_NOT_NULL(list);

    // list_create should allocate 2 blocks: List + sentinel
    TEST_ASSERT_TRUE(alloc_call_count >= 2);

    list_destroy(list, NULL);
}


static void test_append_and_get(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  int a = 1, b = 2, c = 3;
  TEST_ASSERT_TRUE(list_append(list, &a));
  TEST_ASSERT_TRUE(list_append(list, &b));
  TEST_ASSERT_TRUE(list_append(list, &c));
  TEST_ASSERT_EQUAL_PTR(&a, list_get(list, 0));
  TEST_ASSERT_EQUAL_PTR(&b, list_get(list, 1));
  TEST_ASSERT_EQUAL_PTR(&c, list_get(list, 2));
  list_destroy(list, NULL);
}

static void test_insert(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  int a = 1, b = 2, c = 3;
  list_append(list, &a);
  list_append(list, &c);
  TEST_ASSERT_TRUE(list_insert(list, 1, &b));
  TEST_ASSERT_EQUAL_PTR(&a, list_get(list, 0));
  TEST_ASSERT_EQUAL_PTR(&b, list_get(list, 1));
  TEST_ASSERT_EQUAL_PTR(&c, list_get(list, 2));
  list_destroy(list, NULL);
}

static void test_remove(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  int a = 1, b = 2, c = 3;
  list_append(list, &a);
  list_append(list, &b);
  list_append(list, &c);
  void *removed = list_remove(list, 1);
  TEST_ASSERT_EQUAL_PTR(&b, removed);
  TEST_ASSERT_EQUAL_PTR(&a, list_get(list, 0));
  TEST_ASSERT_EQUAL_PTR(&c, list_get(list, 1));
  TEST_ASSERT_EQUAL_UINT32(2, list_size(list));
  list_destroy(list, NULL);
}

static void test_size_and_empty(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  TEST_ASSERT_TRUE(list_is_empty(list));
  int a = 1;
  list_append(list, &a);
  TEST_ASSERT_FALSE(list_is_empty(list));
  TEST_ASSERT_EQUAL_UINT32(1, list_size(list));
  list_remove(list, 0);
  TEST_ASSERT_TRUE(list_is_empty(list));
  list_destroy(list, NULL);
}

static void test_out_of_bounds(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  int a = 1;
  TEST_ASSERT_FALSE(list_insert(list, 1, &a));
  TEST_ASSERT_NULL(list_get(list, 0));
  TEST_ASSERT_NULL(list_remove(list, 0));
  list_destroy(list, NULL);
}

void test_list_create_alloc_failure(void) {
    alloc_fail_after = 1; // fail on first ALLOC (List allocation)
    alloc_call_count = 0;
    TEST_ASSERT_NULL(list_create(LIST_LINKED_SENTINEL));

    alloc_fail_after = 2; // fail on second ALLOC (sentinel allocation)
    alloc_call_count = 0;
    TEST_ASSERT_NULL(list_create(LIST_LINKED_SENTINEL));

    alloc_fail_after = -1; // reset
}

void test_list_append_alloc_failure(void) {
    List *list = list_create(LIST_LINKED_SENTINEL);
    int a = 1;
    alloc_fail_after = 1; // fail on first ALLOC in append
    alloc_call_count = 0;
    TEST_ASSERT_FALSE(list_append(list, &a));
    list_destroy(list, NULL);
    alloc_fail_after = -1; // reset
}
int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_create_and_destroy);
  RUN_TEST(test_append_and_get);
  RUN_TEST(test_insert);
  RUN_TEST(test_remove);
  RUN_TEST(test_size_and_empty);
  RUN_TEST(test_out_of_bounds);
  RUN_TEST(test_alloc_hook_smoke);
  RUN_TEST(test_list_append_alloc_failure);
  RUN_TEST(test_list_create_alloc_failure);
  return UNITY_END();
}
