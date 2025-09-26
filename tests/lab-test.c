
#include <stdlib.h>
#include <stdio.h>
#include "harness/unity.h"
#include <string.h>

//All tests written by AI

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

static int free_count = 0;
static void dummy_free(void *ptr) { (void)ptr; free_count++; }

void test_destroy_calls_destructor(void) {
    List *list = list_create(LIST_LINKED_SENTINEL);
    int a=1, b=2;
    list_append(list, &a);
    list_append(list, &b);

    free_count = 0;
    list_destroy(list, dummy_free);
    TEST_ASSERT_EQUAL_INT(2, free_count);
}

void test_list_insert_alloc_failure(void) {
    List *list = list_create(LIST_LINKED_SENTINEL);
    int a = 1, b = 2;
    list_append(list, &a);

    alloc_fail_after = 1; // fail on allocation inside insert
    alloc_call_count = 0;
    TEST_ASSERT_FALSE(list_insert(list, 1, &b));

    // state unchanged
    TEST_ASSERT_EQUAL_UINT32(1, list_size(list));
    TEST_ASSERT_EQUAL_PTR(&a, list_get(list, 0));

    list_destroy(list, NULL);
    alloc_fail_after = -1;
}

static void test_get_last_index_and_oob(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  int a=1,b=2;
  list_append(list,&a);
  list_append(list,&b);
  TEST_ASSERT_EQUAL_PTR(&b, list_get(list, list_size(list)-1));
  TEST_ASSERT_NULL(list_get(list, list_size(list))); // OOB
  list_destroy(list,NULL);
}

static void test_insert_head_tail_and_remove_to_empty(void) {
  List *list = list_create(LIST_LINKED_SENTINEL);
  int a=1,b=2,c=3;
  TEST_ASSERT_TRUE(list_insert(list, 0, &a));               // head insert into empty
  TEST_ASSERT_TRUE(list_insert(list, list_size(list), &b));  // tail insert
  TEST_ASSERT_TRUE(list_insert(list, 1, &c));                // middle
  // a, c, b
  TEST_ASSERT_EQUAL_PTR(&a, list_get(list, 0));
  TEST_ASSERT_EQUAL_PTR(&c, list_get(list, 1));
  TEST_ASSERT_EQUAL_PTR(&b, list_get(list, 2));

  TEST_ASSERT_EQUAL_PTR(&a, list_remove(list, 0));                         // remove head
  TEST_ASSERT_EQUAL_PTR(&b, list_remove(list, list_size(list)-1));         // remove tail
  TEST_ASSERT_EQUAL_PTR(&c, list_remove(list, 0));                         // last one
  TEST_ASSERT_TRUE(list_is_empty(list));
  list_destroy(list, NULL);
}

static void test_null_list_guards(void) {
  int x=42;
  TEST_ASSERT_FALSE(list_append(NULL,&x));
  TEST_ASSERT_FALSE(list_insert(NULL,0,&x));
  TEST_ASSERT_NULL(list_get(NULL,0));
  TEST_ASSERT_NULL(list_remove(NULL,0));
  TEST_ASSERT_EQUAL_UINT32(0, list_size(NULL));
  TEST_ASSERT_TRUE(list_is_empty(NULL));
}

void test_compare_int(void) {
    int a = 5, b = 10;
    TEST_ASSERT_TRUE(compare_int(&a, &b) > 0); // b > a, descending
    TEST_ASSERT_TRUE(compare_int(&b, &a) < 0); // a < b, descending
    TEST_ASSERT_EQUAL_INT(0, compare_int(&a, &a));
}

void test_compare_str(void) {
    const char *s1 = "apple";
    const char *s2 = "banana";
    TEST_ASSERT_TRUE(compare_str(&s1, &s2) < 0); // apple < banana
    TEST_ASSERT_TRUE(compare_str(&s2, &s1) > 0); // banana > apple
    TEST_ASSERT_EQUAL_INT(0, compare_str(&s1, &s1));
}

void test_list_sort_int(void) {
    List *list = list_create(LIST_LINKED_SENTINEL);
    int vals[] = {5, 2, 9, 1, 7};
    for (int i = 0; i < 5; ++i) list_append(list, &vals[i]);
    list_sort(list, 0, list_size(list), compare_int);
    TEST_ASSERT_TRUE(is_sorted(list, compare_int));
    int expected[] = {9, 7, 5, 2, 1};
    for (int i = 0; i < 5; ++i)
        TEST_ASSERT_EQUAL_INT(expected[i], *(int *)list_get(list, i));
    list_destroy(list, NULL);
}

void test_list_sort_str(void) {
    List *list = list_create(LIST_LINKED_SENTINEL);
    const char *vals[] = {"pear", "apple", "banana", "kiwi"};
    for (int i = 0; i < 4; ++i) list_append(list, (void *)&vals[i]);
    list_sort(list, 0, list_size(list), compare_str);
    TEST_ASSERT_TRUE(is_sorted(list, compare_str));
    const char *expected[] = {"apple", "banana", "kiwi", "pear"};
    for (int i = 0; i < 4; ++i)
        TEST_ASSERT_EQUAL_STRING(expected[i], *(const char **)list_get(list, i));
    list_destroy(list, NULL);
}

void test_list_merge_int(void) {
    List *l1 = list_create(LIST_LINKED_SENTINEL);
    List *l2 = list_create(LIST_LINKED_SENTINEL);
    int a[] = {7, 3, 1};
    int b[] = {8, 6, 2};
    for (int i = 0; i < 3; ++i) list_append(l1, &a[i]);
    for (int i = 0; i < 3; ++i) list_append(l2, &b[i]);
    list_sort(l1, 0, list_size(l1), compare_int);
    list_sort(l2, 0, list_size(l2), compare_int);
    list_merge(l1, l2, compare_int);
    TEST_ASSERT_TRUE(is_sorted(l1, compare_int));
    int expected[] = {8, 7, 6, 3, 2, 1};
    for (int i = 0; i < 6; ++i)
        TEST_ASSERT_EQUAL_INT(expected[i], *(int *)list_get(l1, i));
    TEST_ASSERT_EQUAL_UINT32(0, list_size(l2)); // l2 should be empty
    list_destroy(l1, NULL);
    list_destroy(l2, NULL);
}

void test_is_sorted_false(void) {
    List *list = list_create(LIST_LINKED_SENTINEL);
    int vals[] = {1, 2, 3};
    for (int i = 0; i < 3; ++i) list_append(list, &vals[i]);
    TEST_ASSERT_FALSE(is_sorted(list, compare_int)); // ascending, but compare_int expects descending
    list_destroy(list, NULL);
}

static void test_list_sort_null_cmp_int(void) {
    List *L = list_create(LIST_LINKED_SENTINEL);
    int v[] = {3,1,2};
    for (int i = 0; i < 3; ++i) list_append(L, &v[i]);

    /* Should no-op (guard: !cmp) */
    list_sort(L, 0, list_size(L), NULL);

    /* Verify unchanged order */
    TEST_ASSERT_EQUAL_PTR(&v[0], list_get(L, 0));
    TEST_ASSERT_EQUAL_PTR(&v[1], list_get(L, 1));
    TEST_ASSERT_EQUAL_PTR(&v[2], list_get(L, 2));
    list_destroy(L, NULL);
}

static void test_list_sort_start_eq_end(void) {
    List *L = list_create(LIST_LINKED_SENTINEL);
    int v[] = {4,2,3};
    for (int i = 0; i < 3; ++i) list_append(L, &v[i]);

    /* Guard: start >= end => no-op */
    list_sort(L, 1, 1, compare_int);

    /* Order unchanged */
    TEST_ASSERT_EQUAL_PTR(&v[0], list_get(L, 0));
    TEST_ASSERT_EQUAL_PTR(&v[1], list_get(L, 1));
    TEST_ASSERT_EQUAL_PTR(&v[2], list_get(L, 2));
    list_destroy(L, NULL);
}

static void test_list_sort_end_gt_size_guard(void) {
    List *L = list_create(LIST_LINKED_SENTINEL);
    int v[] = {5, 1};
    for (int i = 0; i < 2; ++i) list_append(L, &v[i]);

    /* Guard: end > size => no-op */
    size_t n = list_size(L); /* == 2 */
    list_sort(L, 0, n + 1, compare_int);

    /* Order unchanged */
    TEST_ASSERT_EQUAL_PTR(&v[0], list_get(L, 0));
    TEST_ASSERT_EQUAL_PTR(&v[1], list_get(L, 1));
    list_destroy(L, NULL);
}

static void test_list_sort_null_list_noop(void) {
    /* Should just return (guard: !list) */
    list_sort(NULL, 0, 0, compare_int);
    /* Nothing to assert; just exercising the branch safely. */
}

static void test_is_sorted_true_int(void) {
    List *L = list_create(LIST_LINKED_SENTINEL);
    int v[] = {9,7,3,1};
    for (int i = 0; i < 4; ++i) list_append(L, &v[i]);
    TEST_ASSERT_TRUE(is_sorted(L, compare_int));
    list_destroy(L, NULL);
}

static void test_is_sorted_true_str(void) {
    List *L = list_create(LIST_LINKED_SENTINEL);
    /* NOTE: your compare_str is used elsewhere with &vals[i], so keep that shape */
    const char *vals[] = {"alpha","bravo","charlie"};
    for (int i = 0; i < 3; ++i) list_append(L, (void *)&vals[i]);
    TEST_ASSERT_TRUE(is_sorted(L, compare_str));
    list_destroy(L, NULL);
}

static void test_merge_left_empty(void) {
    List *l1 = list_create(LIST_LINKED_SENTINEL); /* empty */
    List *l2 = list_create(LIST_LINKED_SENTINEL);
    int b[] = {8,6,2};
    for (int i = 0; i < 3; ++i) list_append(l2, &b[i]);
    list_sort(l2, 0, list_size(l2), compare_int);

    list_merge(l1, l2, compare_int);

    /* l1 now has l2 elements in order; l2 emptied */
    TEST_ASSERT_EQUAL_UINT32(3, list_size(l1));
    TEST_ASSERT_EQUAL_UINT32(0, list_size(l2));
    int expected[] = {8,6,2};
    for (int i = 0; i < 3; ++i)
        TEST_ASSERT_EQUAL_INT(expected[i], *(int*)list_get(l1, i));

    list_destroy(l1, NULL);
    list_destroy(l2, NULL);
}

static void test_merge_right_empty(void) {
    List *l1 = list_create(LIST_LINKED_SENTINEL);
    List *l2 = list_create(LIST_LINKED_SENTINEL); /* empty */
    int a[] = {7,3,1};
    for (int i = 0; i < 3; ++i) list_append(l1, &a[i]);
    list_sort(l1, 0, list_size(l1), compare_int);

    list_merge(l1, l2, compare_int);

    /* l1 unchanged; l2 still empty */
    TEST_ASSERT_EQUAL_UINT32(3, list_size(l1));
    TEST_ASSERT_EQUAL_UINT32(0, list_size(l2));
    int expected[] = {7,3,1};
    for (int i = 0; i < 3; ++i)
        TEST_ASSERT_EQUAL_INT(expected[i], *(int*)list_get(l1, i));

    list_destroy(l1, NULL);
    list_destroy(l2, NULL);
}

static void test_merge_null_cmp_noop(void) {
    List *l1 = list_create(LIST_LINKED_SENTINEL);
    List *l2 = list_create(LIST_LINKED_SENTINEL);
    int a[] = {3}, b[] = {2};
    list_append(l1, &a[0]);
    list_append(l2, &b[0]);

    /* Guard: !cmp => no-op merge */
    list_merge(l1, l2, NULL);

    TEST_ASSERT_EQUAL_UINT32(1, list_size(l1));
    TEST_ASSERT_EQUAL_UINT32(1, list_size(l2));
    TEST_ASSERT_EQUAL_INT(3, *(int*)list_get(l1, 0));
    TEST_ASSERT_EQUAL_INT(2, *(int*)list_get(l2, 0));

    list_destroy(l1, NULL);
    list_destroy(l2, NULL);
}

static void test_destroy_empty_calls_zero(void) {
    List *L = list_create(LIST_LINKED_SENTINEL);
    free_count = 0;
    list_destroy(L, dummy_free);
    TEST_ASSERT_EQUAL_INT(0, free_count); /* empty list => destructor not called */
}

static void test_list_sort_empty_list(void) {
    List *L = list_create(LIST_LINKED_SENTINEL);
    list_sort(L, 0, 0, compare_int);   // count < 2 early return
    TEST_ASSERT_TRUE(list_is_empty(L));
    list_destroy(L, NULL);
}

static void test_list_sort_singleton(void) {
    List *L = list_create(LIST_LINKED_SENTINEL);
    int x = 42; list_append(L, &x);
    list_sort(L, 0, 1, compare_int);   // count == 1 early return
    TEST_ASSERT_EQUAL_PTR(&x, list_get(L, 0));
    list_destroy(L, NULL);
}

/* If your implementation simply returns on start>=end, this hits that branch. */
static void test_list_sort_reversed_range_guard(void) {
    List *L = list_create(LIST_LINKED_SENTINEL);
    int v[] = {2,1}; list_append(L, &v[0]); list_append(L, &v[1]);
    list_sort(L, 2, 1, compare_int);   // start > end => guard path
    TEST_ASSERT_EQUAL_PTR(&v[0], list_get(L, 0));
    TEST_ASSERT_EQUAL_PTR(&v[1], list_get(L, 1));
    list_destroy(L, NULL);
}

static void test_list_sort_null_cmp_str(void) {
    List *L = list_create(LIST_LINKED_SENTINEL);
    const char *vals[] = {"b","a"};
    for (int i = 0; i < 2; ++i) list_append(L, (void*)&vals[i]);
    list_sort(L, 0, list_size(L), NULL);   // !cmp
    TEST_ASSERT_EQUAL_STRING("b", *(const char**)list_get(L,0));
    TEST_ASSERT_EQUAL_STRING("a", *(const char**)list_get(L,1));
    list_destroy(L, NULL);
}

static void test_list_sort_null_list_guard(void) {
    list_sort(NULL, 0, 0, compare_int);    // !list
}

static void test_is_sorted_null_list(void) {
    TEST_ASSERT_FALSE(is_sorted(NULL, compare_int));
}

static void test_is_sorted_null_cmp(void) {
    List *L = list_create(LIST_LINKED_SENTINEL);
    int v[] = {1}; list_append(L, &v[0]);
    TEST_ASSERT_FALSE(is_sorted(L, NULL));
    list_destroy(L, NULL);
}

static void test_merge_both_empty(void) {
    List *a = list_create(LIST_LINKED_SENTINEL);
    List *b = list_create(LIST_LINKED_SENTINEL);
    list_merge(a, b, compare_int);
    TEST_ASSERT_EQUAL_UINT32(0, list_size(a));
    TEST_ASSERT_EQUAL_UINT32(0, list_size(b));
    list_destroy(a, NULL);
    list_destroy(b, NULL);
}

/* Forces the cmp==0 branch at least once */
static void test_merge_equal_int_keys(void) {
    List *l1 = list_create(LIST_LINKED_SENTINEL);
    List *l2 = list_create(LIST_LINKED_SENTINEL);
    int a[] = {5,3};       // descending
    int b[] = {5,4};
    for (int i = 0; i < 2; ++i) list_append(l1, &a[i]);
    for (int i = 0; i < 2; ++i) list_append(l2, &b[i]);
    list_sort(l1, 0, list_size(l1), compare_int);
    list_sort(l2, 0, list_size(l2), compare_int);
    list_merge(l1, l2, compare_int);
    TEST_ASSERT_TRUE(is_sorted(l1, compare_int));
    TEST_ASSERT_EQUAL_UINT32(0, list_size(l2));
    list_destroy(l1, NULL);
    list_destroy(l2, NULL);
}

/* If your merge has guards for NULL lists */
static void test_merge_null_list_guards(void) {
    List *l = list_create(LIST_LINKED_SENTINEL);
    int x = 1; list_append(l, &x);
    list_merge(NULL, l, compare_int);  // expect no crash / no-op
    list_merge(l, NULL, compare_int);  // expect no crash / no-op
    list_destroy(l, NULL);
}

static void test_remove_oob_indices(void) {
    List *L = list_create(LIST_LINKED_SENTINEL);
    int x=1; list_append(L,&x);
    TEST_ASSERT_NULL(list_remove(L, 1));   // == size
    TEST_ASSERT_NULL(list_remove(L, 5));   // > size
    list_destroy(L, NULL);
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
  RUN_TEST(test_destroy_calls_destructor);
  RUN_TEST(test_list_insert_alloc_failure);
  RUN_TEST(test_get_last_index_and_oob);
  RUN_TEST(test_insert_head_tail_and_remove_to_empty);
  RUN_TEST(test_null_list_guards);
  RUN_TEST(test_compare_int);
  RUN_TEST(test_compare_str);
  RUN_TEST(test_list_sort_int);
  RUN_TEST(test_list_sort_str);
  RUN_TEST(test_list_merge_int);
  RUN_TEST(test_is_sorted_false);
  RUN_TEST(test_list_sort_null_cmp_int);
  RUN_TEST(test_list_sort_start_eq_end);
  RUN_TEST(test_list_sort_end_gt_size_guard);
  RUN_TEST(test_list_sort_null_list_noop);
  RUN_TEST(test_is_sorted_true_int);
  RUN_TEST(test_is_sorted_true_str);
  RUN_TEST(test_merge_left_empty);
  RUN_TEST(test_merge_right_empty);
  RUN_TEST(test_merge_null_cmp_noop);
  RUN_TEST(test_destroy_empty_calls_zero);
  RUN_TEST(test_list_sort_empty_list);
  RUN_TEST(test_list_sort_singleton); 
  RUN_TEST(test_list_sort_reversed_range_guard);
  RUN_TEST(test_list_sort_null_cmp_str);
  RUN_TEST(test_list_sort_null_list_guard);
  RUN_TEST(test_is_sorted_null_list);
  RUN_TEST(test_is_sorted_null_cmp);
  RUN_TEST(test_merge_both_empty);
  RUN_TEST(test_merge_equal_int_keys);
  RUN_TEST(test_merge_null_list_guards);
  RUN_TEST(test_remove_oob_indices);
  return UNITY_END();
}
