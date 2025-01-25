#include "check_slist.h"

typedef char string_t;
typedef string_t *pstring;

#define T pstring
#include "slist.h"
#undef T

#define ck_assert_true(x) ck_assert_int_eq(!!(x), 1)
#define ck_assert_false(x) ck_assert_int_eq(!!(x), 0)

static snode_pstring_t *a, *b, *c, *head;

static void setup_nodes(void) {
    a = slist_pstring_create("a");
    b = slist_pstring_create("b");
    c = slist_pstring_create("c");
}

static void setup_slist(void) {
    a = slist_pstring_create("a");
    b = slist_pstring_create("b");
    c = slist_pstring_create("c");
    head = slist_pstring_concat(b, c);
    head = slist_pstring_concat(a, head);
}

static void teardown_nodes(void) {
    free(a);
    a = NULL;
    free(b);
    b = NULL;
    free(c);
    c = NULL;
}

static void teardown_slist(void) {
    slist_pstring_destroy(head);
}

START_TEST (test_slist_create) {
    ck_assert_ptr_nonnull(a);
    ck_assert_str_eq(slist_pstring_entry(a), "a");
    ck_assert_ptr_null(a->next);
    ck_assert_true(slist_pstring_is_last(a));
} END_TEST

TCase *check_slist_create(void) {
    TCase *tc = tcase_create("check_slist_create");
    tcase_add_checked_fixture(tc, setup_nodes, teardown_nodes);
    tcase_add_test(tc, test_slist_create);
    return tc;
}

START_TEST (test_slist_concat_two) {
    const snode_pstring_t *head = slist_pstring_concat(a, b);
    ck_assert_ptr_eq(head, a);
    ck_assert_ptr_eq(head->next, b);
    ck_assert_false(slist_pstring_is_last(head));
    ck_assert_true(slist_pstring_is_last(b));
} END_TEST

TCase *check_slist_concat_two(void) {
    TCase *tc = tcase_create("check_slist_concat_two");
    tcase_add_checked_fixture(tc, setup_nodes, teardown_nodes);
    tcase_add_test(tc, test_slist_concat_two);
    return tc;
}

START_TEST (test_slist_concat_three_simple) {
    snode_pstring_t *head = slist_pstring_concat(b, c);
    ck_assert_ptr_eq(head, b);
    ck_assert_ptr_eq(head->next, c);
    head = slist_pstring_concat(a, head);
    ck_assert_ptr_eq(head, a);
    ck_assert_ptr_eq(head->next, b);
    ck_assert_ptr_eq(head->next->next, c);
} END_TEST

TCase *check_slist_concat_three_simple(void) {
    TCase *tc = tcase_create("check_slist_concat_three_simple");
    tcase_add_checked_fixture(tc, setup_nodes, teardown_nodes);
    tcase_add_test(tc, test_slist_concat_three_simple);
    return tc;
}

START_TEST (test_slist_concat_three_with_loop) {
    snode_pstring_t *head = slist_pstring_concat(a, b);
    ck_assert_ptr_eq(head, a);
    ck_assert_ptr_eq(head->next, b);
    head = slist_pstring_concat(head, c);
    ck_assert_ptr_eq(head, a);
    ck_assert_ptr_eq(head->next, b);
    ck_assert_ptr_eq(head->next->next, c);
} END_TEST

TCase *check_slist_concat_three_with_loop(void) {
    TCase *tc = tcase_create("check_slist_concat_three_with_loop");
    tcase_add_checked_fixture(tc, setup_nodes, teardown_nodes);
    tcase_add_test(tc, test_slist_concat_three_with_loop);
    return tc;
}

START_TEST (test_slist_concat_with_null) {
    const snode_pstring_t *head = slist_pstring_concat(a, NULL);
    ck_assert_ptr_eq(head, a);
    ck_assert_ptr_eq(head->next, NULL);
    ck_assert_true(slist_pstring_is_last(head));
} END_TEST

TCase *check_slist_concat_with_null(void) {
    TCase *tc = tcase_create("check_slist_concat_with_null");
    tcase_add_checked_fixture(tc, setup_nodes, teardown_nodes);
    tcase_add_test(tc, test_slist_concat_with_null);
    return tc;
}

START_TEST (test_slist_concat_null) {
    const snode_pstring_t *head = slist_pstring_concat(NULL, a);
    ck_assert_ptr_eq(head, a);
    ck_assert_ptr_eq(head->next, NULL);
    ck_assert_true(slist_pstring_is_last(head));
} END_TEST

TCase *check_slist_concat_null(void) {
    TCase *tc = tcase_create("check_slist_concat_null");
    tcase_add_checked_fixture(tc, setup_nodes, teardown_nodes);
    tcase_add_test(tc, test_slist_concat_null);
    return tc;
}

START_TEST (test_slist_concat_null_with_null) {
    const snode_pstring_t *head = slist_pstring_concat(NULL, NULL);
    ck_assert_ptr_null(head);
} END_TEST

TCase *check_slist_concat_null_with_null(void) {
    TCase *tc = tcase_create("check_slist_concat_null_with_null");
    tcase_add_test(tc, test_slist_concat_null_with_null);
    return tc;
}

START_TEST (test_slist_for_each) {
    const string_t *ordered[] = { "a", "b", "c" };
    int i = 0;

    snode_pstring_t *node;
    slist_for_each(head, node) {
        ck_assert_str_eq(slist_pstring_entry(node), ordered[i]);
        i++;
    }

    ck_assert_ptr_null(node);
    ck_assert_int_eq(i, sizeof ordered / sizeof *ordered);
} END_TEST

TCase *check_slist_for_each(void) {
    TCase *tc = tcase_create("check_slist_for_each");
    tcase_add_checked_fixture(tc, setup_slist, teardown_slist);
    tcase_add_test(tc, test_slist_for_each);
    return tc;
}

START_TEST (test_slist_for_each_safe) {
    const string_t *ordered[] = { "a", "b", "c" };
    int i = 0;

    snode_pstring_t *node, *n;
    slist_for_each_safe(head, node, n) {
        node->next = NULL;  // попытка "сломать" цикл изнутри.
        ck_assert_str_eq(slist_pstring_entry(node), ordered[i]);
        i++;
    }

    ck_assert_ptr_null(node);
    ck_assert_int_eq(i, sizeof ordered / sizeof *ordered);
} END_TEST

TCase *check_slist_for_each_safe(void) {
    TCase *tc = tcase_create("check_slist_for_each_safe");
    tcase_add_checked_fixture(tc, setup_slist, teardown_slist);
    tcase_add_test(tc, test_slist_for_each_safe);
    return tc;
}

Suite *check_slist_suite(void) {
    Suite *suite = suite_create("check_slist_suite");
    suite_add_tcase(suite, check_slist_create());
    suite_add_tcase(suite, check_slist_concat_two());
    suite_add_tcase(suite, check_slist_concat_three_simple());
    suite_add_tcase(suite, check_slist_concat_three_with_loop());
    suite_add_tcase(suite, check_slist_concat_with_null());
    suite_add_tcase(suite, check_slist_concat_null());
    suite_add_tcase(suite, check_slist_concat_null_with_null());
    suite_add_tcase(suite, check_slist_for_each());
    suite_add_tcase(suite, check_slist_for_each_safe());
    return suite;
}
