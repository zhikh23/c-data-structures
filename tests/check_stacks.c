#include "check_stacks.h"

#include "astack.h"
#include "err.h"
#include "lstack.h"
#include "stack.h"


#define ck_assert_true(x) ck_assert_int_eq(!!(x), 1)
#define ck_assert_false(x) ck_assert_int_eq(!!(x), 0)

#define LEN(arr) (sizeof(arr) / sizeof(arr[0]))


static void *stack;


void setup_astack(void) {
    stack = stack_new(ArrayStack);
}

void setup_lstack(void) {
    stack = stack_new(ListStack);
}

void teardown_stack(void) {
    stack_destroy(stack);
    stack = NULL;
}


START_TEST (test_stack_push_pop) {
    ck_assert_true(stack_empty(stack));
    const int err = stack_push(stack, 42);
    ck_assert_false(IS_ERR(err));
    ck_assert_false(stack_empty(stack));

    const sval_t popped = stack_pop(stack);
    ck_assert_int_eq(popped, 42);
    ck_assert_true(stack_empty(stack));
} END_TEST

START_TEST (test_stack_push_peek_pop) {
    ck_assert_true(stack_empty(stack));
    const int err = stack_push(stack, 42);
    ck_assert_false(IS_ERR(err));
    ck_assert_false(stack_empty(stack));

    const sval_t peeked = stack_peek(stack);
    ck_assert_int_eq(peeked, 42);
    ck_assert_false(stack_empty(stack));

    const sval_t popped = stack_pop(stack);
    ck_assert_int_eq(popped, 42);
    ck_assert_true(stack_empty(stack));
} END_TEST

START_TEST (test_stack_push_many_pop_all) {
    static int nums[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    for (int i = 0; i < LEN(nums); i++) {
        const int err = stack_push(stack, nums[i]);
        ck_assert_false(IS_ERR(err));
    }

    for (int i = LEN(nums) - 1; i >= 0; i--) {
        const sval_t popped = stack_pop(stack);
        ck_assert_int_eq(popped, nums[i]);
    }

    ck_assert_true(stack_empty(stack));
} END_TEST

TCase *check_astack_push_pop(void) {
    TCase *tc = tcase_create("check_astack_push_pop");
    tcase_add_unchecked_fixture(tc, setup_astack, teardown_stack);
    tcase_add_test(tc, test_stack_push_pop);
    return tc;
}

TCase *check_astack_push_peek_pop(void) {
    TCase *tc = tcase_create("check_astack_push_peek_pop");
    tcase_add_unchecked_fixture(tc, setup_astack, teardown_stack);
    tcase_add_test(tc, test_stack_push_peek_pop);
    return tc;
}

TCase *check_astack_push_many_pop_all(void) {
    TCase *tc = tcase_create("check_astack_push_many_pop_all");
    tcase_add_unchecked_fixture(tc, setup_astack, teardown_stack);
    tcase_add_test(tc, test_stack_push_many_pop_all);
    return tc;
}

Suite *check_astack_suite(void) {
    Suite *suite = suite_create("check_astack_suite");
    suite_add_tcase(suite, check_astack_push_pop());
    suite_add_tcase(suite, check_astack_push_peek_pop());
    suite_add_tcase(suite, check_astack_push_many_pop_all());
    return suite;
}

TCase *check_lstack_push_pop(void) {
    TCase *tc = tcase_create("check_lstack_push_pop");
    tcase_add_unchecked_fixture(tc, setup_lstack, teardown_stack);
    tcase_add_test(tc, test_stack_push_pop);
    return tc;
}

TCase *check_lstack_push_peek_pop(void) {
    TCase *tc = tcase_create("check_lstack_push_peek_pop");
    tcase_add_unchecked_fixture(tc, setup_lstack, teardown_stack);
    tcase_add_test(tc, test_stack_push_peek_pop);
    return tc;
}

TCase *check_lstack_push_many_pop_all(void) {
    TCase *tc = tcase_create("check_lstack_push_many_pop_all");
    tcase_add_unchecked_fixture(tc, setup_lstack, teardown_stack);
    tcase_add_test(tc, test_stack_push_many_pop_all);
    return tc;
}

Suite *check_lstack_suite(void) {
    Suite *suite = suite_create("check_lstack_suite");
    suite_add_tcase(suite, check_lstack_push_pop());
    suite_add_tcase(suite, check_lstack_push_peek_pop());
    suite_add_tcase(suite, check_lstack_push_many_pop_all());
    return suite;
}
