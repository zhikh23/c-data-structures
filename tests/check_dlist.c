#include "check_dlist.h"

typedef char string_t;
typedef string_t *pstring;

#define T pstring
#include "dlist.h"
#undef T

#define ck_assert_true(x) ck_assert_int_eq(!!(x), 1)
#define ck_assert_false(x) ck_assert_int_eq(!!(x), 0)

#define LEN(arr) (sizeof(arr) / sizeof(arr[0]))

static dnode_pstring_t *a, *b, *c, *head;


static void setup_nodes(void) {
    a = dlist_pstring_create("a");
    b = dlist_pstring_create("b");
    c = dlist_pstring_create("c");
}

static void setup_dlist(void) {
    a = dlist_pstring_create("a");
    b = dlist_pstring_create("b");
    c = dlist_pstring_create("c");
    head = a;
    head = dlist_pstring_add_tail(head, b);
    head = dlist_pstring_add_tail(head, c);
}

static void teardown_nodes(void) {
    free(a);
    a = NULL;
    free(b);
    b = NULL;
    free(c);
    c = NULL;
}

static void teardown_dlist(void) {
    dlist_pstring_destroy(head);
}

START_TEST (test_dlist_create) {
    ck_assert_ptr_nonnull(a);
    ck_assert_str_eq(dlist_pstring_entry(a), "a");
    ck_assert_ptr_eq(a->next, a);
} END_TEST

TCase *check_dlist_create(void) {
    TCase *tc = tcase_create("check_dlist_create");
    tcase_add_checked_fixture(tc, setup_nodes, teardown_nodes);
    tcase_add_test(tc, test_dlist_create);
    return tc;
}

START_TEST (test_dlist_add_tail) {
    head = a;
    head = dlist_pstring_add_tail(head, b);

    ck_assert_ptr_eq(head, a);

    ck_assert_true(dlist_pstring_is_last(head, b));
    ck_assert_false(dlist_pstring_is_last(head, a));

    ck_assert_true(dlist_pstring_is_first(head, b));
    ck_assert_false(dlist_pstring_is_first(head, a));
} END_TEST

TCase *check_dlist_add_tail(void) {
    TCase *tc = tcase_create("check_dlist_add_tail");
    tcase_add_checked_fixture(tc, setup_nodes, teardown_nodes);
    tcase_add_test(tc, test_dlist_add_tail);
    return tc;
}

START_TEST (test_dlist_add_tail_in_loop) {
    dnode_pstring_t *nodes[] = { a, b, c };
    head = NULL;

    for (dnode_pstring_t **pnode = nodes; pnode < nodes + LEN(nodes); pnode++)
        head = dlist_pstring_add_tail(head, *pnode);

    ck_assert_ptr_eq(head, a);

    ck_assert_true(dlist_pstring_is_last(head, c));
    ck_assert_false(dlist_pstring_is_last(head, b));
    ck_assert_false(dlist_pstring_is_last(head, a));

    ck_assert_false(dlist_pstring_is_first(head, c));
    ck_assert_true(dlist_pstring_is_first(head, b));
    ck_assert_false(dlist_pstring_is_first(head, a));
} END_TEST

TCase *check_dlist_add_tail_in_loop(void) {
    TCase *tc = tcase_create("check_dlist_add_tail_in_loop");
    tcase_add_checked_fixture(tc, setup_nodes, teardown_nodes);
    tcase_add_test(tc, test_dlist_add_tail_in_loop);
    return tc;
}

START_TEST (test_dlist_add_head) {
    head = a;
    head = dlist_pstring_add_head(b, head);

    ck_assert_ptr_eq(head, b);

    ck_assert_true(dlist_pstring_is_last(head, a));
    ck_assert_false(dlist_pstring_is_last(head, b));

    ck_assert_true(dlist_pstring_is_first(head, a));
    ck_assert_false(dlist_pstring_is_first(head, b));
} END_TEST

TCase *check_dlist_add_head(void) {
    TCase *tc = tcase_create("check_dlist_add_head");
    tcase_add_checked_fixture(tc, setup_nodes, teardown_nodes);
    tcase_add_test(tc, test_dlist_add_head);
    return tc;
}

START_TEST (test_dlist_add_head_in_loop) {
    dnode_pstring_t *nodes[] = { a, b, c };
    head = NULL;

    for (dnode_pstring_t **pnode = nodes; pnode < nodes + LEN(nodes); pnode++)
        head = dlist_pstring_add_head(*pnode, head);

    ck_assert_ptr_eq(head, c);

    ck_assert_true(dlist_pstring_is_last(head, a));
    ck_assert_false(dlist_pstring_is_last(head, b));
    ck_assert_false(dlist_pstring_is_last(head, c));

    ck_assert_false(dlist_pstring_is_first(head, a));
    ck_assert_true(dlist_pstring_is_first(head, b));
    ck_assert_false(dlist_pstring_is_first(head, c));
} END_TEST

TCase *check_dlist_add_head_in_loop(void) {
    TCase *tc = tcase_create("check_dlist_add_head_in_loop");
    tcase_add_checked_fixture(tc, setup_nodes, teardown_nodes);
    tcase_add_test(tc, test_dlist_add_head_in_loop);
    return tc;
}

START_TEST (test_dlist_for_each) {
    const string_t *ordered[] = { "b", "c" };
    int i = 0;

    dnode_pstring_t *node;
    dlist_for_each(head, node) {
        ck_assert_str_eq(dlist_pstring_entry(node), ordered[i]);
        i++;
        ck_assert_ptr_eq(node->prev->next, node);
        ck_assert_ptr_eq(node->next->prev, node);
    }

    ck_assert_ptr_eq(node, head);
    ck_assert_int_eq(i, sizeof ordered / sizeof *ordered);
} END_TEST

TCase *check_dlist_for_each(void) {
    TCase *tc = tcase_create("check_dlist_for_each");
    tcase_add_checked_fixture(tc, setup_dlist, teardown_dlist);
    tcase_add_test(tc, test_dlist_for_each);
    return tc;
}

START_TEST (test_dlist_for_each_prev) {
    const string_t *ordered[] = { "c", "b" };
    int i = 0;

    dnode_pstring_t *node;
    dlist_for_each_prev(head, node) {
        ck_assert_str_eq(dlist_pstring_entry(node), ordered[i]);
        i++;
        ck_assert_ptr_eq(node->prev->next, node);
        ck_assert_ptr_eq(node->next->prev, node);
    }

    ck_assert_ptr_eq(node, head);
    ck_assert_int_eq(i, sizeof ordered / sizeof *ordered);
} END_TEST

TCase *check_dlist_for_each_prev(void) {
    TCase *tc = tcase_create("check_dlist_for_each_prev");
    tcase_add_checked_fixture(tc, setup_dlist, teardown_dlist);
    tcase_add_test(tc, test_dlist_for_each_prev);
    return tc;
}

START_TEST (test_dlist_for_each_safe) {
    const string_t *ordered[] = { "b", "c" };
    int i = 0;

    dnode_pstring_t *node, *n;
    dlist_for_each_safe(head, node, n) {
        node->next = NULL;  // попытка "сломать" цикл изнутри.
        ck_assert_str_eq(dlist_pstring_entry(node), ordered[i]);
        i++;
    }

    ck_assert_ptr_eq(node, head);
    ck_assert_int_eq(i, LEN(ordered));
} END_TEST

TCase *check_dlist_for_each_safe(void) {
    TCase *tc = tcase_create("check_dlist_for_each_safe");
    tcase_add_checked_fixture(tc, setup_dlist, teardown_nodes);
    tcase_add_test(tc, test_dlist_for_each_safe);
    return tc;
}

START_TEST (test_dlist_for_each_prev_safe) {
    const string_t *ordered[] = { "c", "b" };
    int i = 0;

    dnode_pstring_t *node, *n;
    dlist_for_each_prev_safe(head, node, n) {
        node->next = NULL;  // попытка "сломать" цикл изнутри.
        ck_assert_str_eq(dlist_pstring_entry(node), ordered[i]);
        i++;
    }

    ck_assert_ptr_eq(node, head);
    ck_assert_int_eq(i, LEN(ordered));
} END_TEST

TCase *check_dlist_for_each_prev_safe(void) {
    TCase *tc = tcase_create("check_dlist_for_each_prev_safe");
    tcase_add_checked_fixture(tc, setup_dlist, teardown_nodes);
    tcase_add_test(tc, test_dlist_for_each_prev_safe);
    return tc;
}

Suite *check_dlist_suite(void) {
    Suite *suite = suite_create("check_dlist_suite");
    suite_add_tcase(suite, check_dlist_create());
    suite_add_tcase(suite, check_dlist_add_tail());
    suite_add_tcase(suite, check_dlist_add_tail_in_loop());
    suite_add_tcase(suite, check_dlist_add_head());
    suite_add_tcase(suite, check_dlist_add_head_in_loop());
    suite_add_tcase(suite, check_dlist_for_each());
    suite_add_tcase(suite, check_dlist_for_each_safe());
    suite_add_tcase(suite, check_dlist_for_each_prev());
    suite_add_tcase(suite, check_dlist_for_each_prev_safe());
    return suite;
}
