#include "check_vec.h"

#define T int
#include "vec.h"
#undef T

#define ck_assert_true(x) ck_assert_int_eq(!!(x), 1)
#define ck_assert_false(x) ck_assert_int_eq(!!(x), 0)

static vec_int_t *v;

void setup_vec_empty(void) {
    v = vec_int_create(0);
}

void setup_vec_fill_ordered(void) {
    const int l = 5;
    v = vec_int_create(l);
    for (int i = 0; i < l; i++)
        v = vec_int_push(v, i);
}

void teardown_vec(void) {
    free(v);
}

START_TEST (test_vec_create) {
    ck_assert_ptr_nonnull(v);
    ck_assert_false(IS_ERR(v));
    ck_assert_int_eq(v->cap, 0);
    ck_assert_int_eq(v->len, 0);
} END_TEST

TCase* check_vec_create_tcase(void) {
    TCase *tc = tcase_create("check_vec_create_tcase");
    tcase_add_checked_fixture(tc, setup_vec_empty, teardown_vec);
    tcase_add_test(tc, test_vec_create);
    return tc;
}

START_TEST (test_vec_push) {
    for (int i = 0; i < 5; i++) {
        v = vec_int_push(v, i);
        ck_assert_false(IS_ERR(v));
        ck_assert_int_eq(v->len, i + 1);
        ck_assert_int_ge(v->cap, i + 1);
        ck_assert_int_eq(vec_int_entry(v, i), i);
    }
} END_TEST

TCase* test_vec_push_tcase(void) {
    TCase *tc = tcase_create("check_vec_push_tcase");
    tcase_add_checked_fixture(tc, setup_vec_empty, teardown_vec);
    tcase_add_test(tc, test_vec_push);
    return tc;
}

START_TEST (test_vec_pop) {
    const int l = (int)vec_int_len(v);
    for (int i = l; i > 0; i--) {
        v = vec_int_pop(v);
        ck_assert_false(IS_ERR(v));
        ck_assert_int_eq(v->len, i - 1);
        ck_assert_int_ge(v->cap, i - 1);
    }
    ck_assert_true(vec_int_is_empty(v));
} END_TEST

TCase* test_vec_pop_tcase(void) {
    TCase *tc = tcase_create("check_vec_pop_tcase");
    tcase_add_checked_fixture(tc, setup_vec_fill_ordered, teardown_vec);
    tcase_add_test(tc, test_vec_pop);
    return tc;
}

START_TEST (test_vec_for_each) {
    int i = 0;
    int *it;
    vec_for_each(v, it) {
        ck_assert_int_eq(*it, i);
        i++;
    }
} END_TEST

TCase* test_vec_for_each_tcase(void) {
    TCase *tc = tcase_create("check_vec_for_each_tcase");
    tcase_add_checked_fixture(tc, setup_vec_fill_ordered, teardown_vec);
    tcase_add_test(tc, test_vec_for_each);
    return tc;
}

Suite *check_vec_suite(void) {
    Suite *suite = suite_create("check_vec_suite");
    suite_add_tcase(suite, check_vec_create_tcase());
    suite_add_tcase(suite, test_vec_push_tcase());
    suite_add_tcase(suite, test_vec_pop_tcase());
    suite_add_tcase(suite, test_vec_for_each_tcase());
    return suite;
}
