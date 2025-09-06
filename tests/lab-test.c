
#include <stdlib.h>
#include <stdio.h>
#include "harness/unity.h"
#include "../src/lab.h"


void setUp(void) {}
void tearDown(void) {}

static void test_create_and_destroy(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  TEST_ASSERT_NOT_NULL(list);
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

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_create_and_destroy);
  RUN_TEST(test_append_and_get);
  RUN_TEST(test_insert);
  RUN_TEST(test_remove);
  RUN_TEST(test_size_and_empty);
  RUN_TEST(test_out_of_bounds);
  return UNITY_END();
}
