#include "check_matrices.h"

#include "csr_matrix.h"
#include "err.h"
#include "flat_matrix.h"
#include "matrix.h"


#define ck_assert_true(x) ck_assert_int_eq(!!(x), 1)
#define ck_assert_false(x) ck_assert_int_eq(!!(x), 0)

#define LEN(arr) (sizeof(arr) / sizeof(arr[0]))


static void *mtx, *aug;


void teardown_matrix(void) {
    if (!IS_ERR(mtx))
        matrix_destroy(mtx);
}

void teardown_aug(void) {
    if (!IS_ERR(aug))
        matrix_destroy(aug);
}


void setup_flat_matrix_1x1(void) {
    mtx = matrix_new(FlatMatrix, 1, 1);
}

void setup_flat_matrix_1x2(void) {
    mtx = matrix_new(FlatMatrix, 1, 2);
}

void setup_flat_matrix_1x0(void) {
    mtx = matrix_new(FlatMatrix, 1, 0);
}

void setup_flat_matrix_0x1(void) {
    mtx = matrix_new(FlatMatrix, 0, 1);
}

void setup_flat_matrix_0x0(void) {
    mtx = matrix_new(FlatMatrix, 0, 0);
}

void setup_flat_matrix_3x3(void) {
    mtx = matrix_new(FlatMatrix, 3, 3);
}

void setup_flat_matrix_100x100(void) {
    mtx = matrix_new(FlatMatrix, 100, 100);
}

void setup_aug_flat_matrix_3x3(void) {
    aug = matrix_new(FlatMatrix, 3, 3);
}

void setup_aug_flat_matrix_100x100(void) {
    aug = matrix_new(FlatMatrix, 100, 100);
}


void setup_csr_matrix_1x1(void) {
    mtx = matrix_new(CSRMatrix, 1, 1);
}

void setup_csr_matrix_1x2(void) {
    mtx = matrix_new(CSRMatrix, 1, 2);
}

void setup_csr_matrix_1x0(void) {
    mtx = matrix_new(CSRMatrix, 1, 0);
}

void setup_csr_matrix_0x1(void) {
    mtx = matrix_new(CSRMatrix, 0, 1);
}

void setup_csr_matrix_0x0(void) {
    mtx = matrix_new(CSRMatrix, 0, 0);
}

void setup_csr_matrix_3x3(void) {
    mtx = matrix_new(CSRMatrix, 3, 3);
}

void setup_csr_matrix_100x100(void) {
    mtx = matrix_new(CSRMatrix, 100, 100);
}

void setup_aug_csr_matrix_3x3(void) {
    aug = matrix_new(CSRMatrix, 3, 3);
}

void setup_aug_csr_matrix_100x100(void) {
    aug = matrix_new(CSRMatrix, 100, 100);
}



START_TEST (test_matrix_is_created) {
    ck_assert_false(IS_ERR(mtx));
} END_TEST

START_TEST (test_matrix_einval_error) {
    ck_assert_true(IS_ERR(mtx));
    ck_assert_int_eq(PTR_ERR(mtx), EINVAL);
} END_TEST

START_TEST (test_matrix_has_1x1_size) {
    ck_assert_int_eq(matrix_rows(mtx), 1);
    ck_assert_int_eq(matrix_cols(mtx), 1);
} END_TEST

START_TEST (test_matrix_has_1x2_size) {
    ck_assert_int_eq(matrix_rows(mtx), 1);
    ck_assert_int_eq(matrix_cols(mtx), 2);
} END_TEST

START_TEST (test_matrix_sum_100x100_zero_and_zero) {
    void *sum = matrix_sum(mtx, aug);
    ck_assert_false(IS_ERR(sum));

    // Проверяем, что результат тоже нулевая матрица.
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++)
            ck_assert_double_eq(matrix_get(sum, i, j), 0.0);
    }

    matrix_destroy(sum);
} END_TEST

START_TEST (test_matrix_sum_100x100_zero_and_nonzero) {
    matrix_set(aug, 1, 1, 1.0);
    matrix_set(aug, 1, 20, 2.0);
    matrix_set(aug, 30, 2, 3.0);

    void *sum = matrix_sum(mtx, aug);
    ck_assert_false(IS_ERR(sum));

    ck_assert_double_eq(matrix_get(sum, 1, 1), 1.0);
    ck_assert_double_eq(matrix_get(sum, 1, 20), 2.0);
    ck_assert_double_eq(matrix_get(sum, 30, 2), 3.0);

    matrix_destroy(sum);
} END_TEST

START_TEST (test_matrix_sum_100x100_nonzero_and_zero) {
    matrix_set(mtx, 1, 1, 1.0);
    matrix_set(mtx, 1, 20, 2.0);
    matrix_set(mtx, 30, 2, 3.0);

    void *sum = matrix_sum(mtx, aug);
    ck_assert_false(IS_ERR(sum));

    ck_assert_double_eq(matrix_get(sum, 1, 1), 1.0);
    ck_assert_double_eq(matrix_get(sum, 1, 20), 2.0);
    ck_assert_double_eq(matrix_get(sum, 30, 2), 3.0);

    matrix_destroy(sum);
} END_TEST

START_TEST (test_matrix_sum_3x3_common) {
    matrix_fill(mtx,
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    );
    matrix_fill(aug,
        9.0, 8.0, 7.0,
        6.0, 5.0, 4.0,
        3.0, 2.0, 1.0
    );

    void *sum = matrix_sum(mtx, aug);
    ck_assert_false(IS_ERR(sum));

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++)
            ck_assert_double_eq(matrix_get(sum, i, j), 10.0);
    }

    matrix_destroy(sum);
} END_TEST

START_TEST (test_matrix_sum_invalid_sizes) {
    void *sum = matrix_sum(mtx, aug);
    ck_assert_int_eq(PTR_ERR_OR_ZERO(sum), EINVAL);
} END_TEST

START_TEST (test_matrix_sum_3x3_zero_result) {
    matrix_fill(mtx,
        1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0
    );
    matrix_fill(aug,
        -1.0, -2.0, -3.0,
        -4.0, -5.0, -6.0,
        -7.0, -8.0, -9.0
    );

    void *sum = matrix_sum(mtx, aug);
    ck_assert_false(IS_ERR(sum));

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++)
            ck_assert_double_eq(matrix_get(sum, i, j), 0.0);
    }

    matrix_destroy(sum);
} END_TEST


TCase* check_flat_matrix_invalid_0x0_size(void) {
    TCase* tc = tcase_create("check_flat_matrix_invalid_0x0_size");
    tcase_add_unchecked_fixture(tc, setup_flat_matrix_0x0, teardown_matrix);
    tcase_add_test(tc, test_matrix_einval_error);
    return tc;
}

TCase* check_flat_matrix_invalid_0x1_size(void) {
    TCase* tc = tcase_create("check_flat_matrix_invalid_0x1_size");
    tcase_add_unchecked_fixture(tc, setup_flat_matrix_0x1, teardown_matrix);
    tcase_add_test(tc, test_matrix_einval_error);
    return tc;
}

TCase* check_flat_matrix_invalid_1x0_size(void) {
    TCase* tc = tcase_create("check_flat_matrix_invalid_1x0_size");
    tcase_add_unchecked_fixture(tc, setup_flat_matrix_1x0, teardown_matrix);
    tcase_add_test(tc, test_matrix_einval_error);
    return tc;
}

TCase* check_flat_matrix_create_1x1(void) {
    TCase* tc = tcase_create("check_flat_matrix_create_1x1");
    tcase_add_unchecked_fixture(tc, setup_flat_matrix_1x1, teardown_matrix);
    tcase_add_test(tc, test_matrix_is_created);
    tcase_add_test(tc, test_matrix_has_1x1_size);
    return tc;
}

TCase* check_flat_matrix_create_1x2(void) {
    TCase* tc = tcase_create("check_flat_matrix_create_1x2");
    tcase_add_unchecked_fixture(tc, setup_flat_matrix_1x2, teardown_matrix);
    tcase_add_test(tc, test_matrix_is_created);
    tcase_add_test(tc, test_matrix_has_1x2_size);
    return tc;
}

TCase *check_flat_matrix_sum_3x3_common(void) {
    TCase* tc = tcase_create("check_flat_matrix_sum_3x3_common");
    tcase_add_unchecked_fixture(tc, setup_flat_matrix_3x3, teardown_matrix);
    tcase_add_unchecked_fixture(tc, setup_aug_flat_matrix_3x3, teardown_aug);
    tcase_add_test(tc, test_matrix_sum_3x3_common);
    return tc;
}

TCase *check_flat_matrix_sum_3x3_zero_result(void) {
    TCase* tc = tcase_create("check_flat_matrix_sum_3x3_zero_result");
    tcase_add_unchecked_fixture(tc, setup_flat_matrix_3x3, teardown_matrix);
    tcase_add_unchecked_fixture(tc, setup_aug_flat_matrix_3x3, teardown_aug);
    tcase_add_test(tc, test_matrix_sum_3x3_zero_result);
    return tc;
}

TCase *check_flat_matrix_sum_invalid_sizes(void) {
    TCase* tc = tcase_create("check_flat_matrix_sum_invalid_sizes");
    tcase_add_unchecked_fixture(tc, setup_flat_matrix_3x3, teardown_matrix);
    tcase_add_unchecked_fixture(tc, setup_aug_flat_matrix_100x100, teardown_aug);
    tcase_add_test(tc, test_matrix_sum_invalid_sizes);
    return tc;
}

Suite *check_flat_matrix_suite(void) {
    Suite* suite = suite_create("check_flat_matrix_suite");
    suite_add_tcase(suite, check_flat_matrix_invalid_0x0_size());
    suite_add_tcase(suite, check_flat_matrix_invalid_0x1_size());
    suite_add_tcase(suite, check_flat_matrix_invalid_1x0_size());
    suite_add_tcase(suite, check_flat_matrix_create_1x1());
    suite_add_tcase(suite, check_flat_matrix_create_1x2());
    suite_add_tcase(suite, check_flat_matrix_sum_3x3_common());
    suite_add_tcase(suite, check_flat_matrix_sum_invalid_sizes());
    suite_add_tcase(suite, check_flat_matrix_sum_3x3_zero_result());
    return suite;
}


TCase* check_csr_matrix_invalid_0x0_size(void) {
    TCase* tc = tcase_create("check_csr_matrix_invalid_0x0_size");
    tcase_add_unchecked_fixture(tc, setup_csr_matrix_0x0, teardown_matrix);
    tcase_add_test(tc, test_matrix_einval_error);
    return tc;
}

TCase* check_csr_matrix_invalid_0x1_size(void) {
    TCase* tc = tcase_create("check_csr_matrix_invalid_0x1_size");
    tcase_add_unchecked_fixture(tc, setup_csr_matrix_0x1, teardown_matrix);
    tcase_add_test(tc, test_matrix_einval_error);
    return tc;
}

TCase* check_csr_matrix_invalid_1x0_size(void) {
    TCase* tc = tcase_create("check_csr_matrix_invalid_1x0_size");
    tcase_add_unchecked_fixture(tc, setup_csr_matrix_1x0, teardown_matrix);
    tcase_add_test(tc, test_matrix_einval_error);
    return tc;
}

TCase* check_csr_matrix_create_1x1(void) {
    TCase* tc = tcase_create("check_csr_matrix_create_1x1");
    tcase_add_unchecked_fixture(tc, setup_csr_matrix_1x1, teardown_matrix);
    tcase_add_test(tc, test_matrix_is_created);
    tcase_add_test(tc, test_matrix_has_1x1_size);
    return tc;
}

TCase* check_csr_matrix_create_1x2(void) {
    TCase* tc = tcase_create("check_csr_matrix_create_1x2");
    tcase_add_unchecked_fixture(tc, setup_csr_matrix_1x2, teardown_matrix);
    tcase_add_test(tc, test_matrix_is_created);
    tcase_add_test(tc, test_matrix_has_1x2_size);
    return tc;
}

TCase *check_csr_matrix_sum_100x100_zero_and_zero(void) {
    TCase* tc = tcase_create("check_csr_matrix_sum_100x100_zero_and_zero");
    tcase_add_unchecked_fixture(tc, setup_csr_matrix_100x100, teardown_matrix);
    tcase_add_unchecked_fixture(tc, setup_aug_csr_matrix_100x100, teardown_aug);
    tcase_add_test(tc, test_matrix_sum_100x100_zero_and_zero);
    return tc;
}

TCase *check_csr_matrix_sum_100x100_zero_and_nonzero(void) {
    TCase* tc = tcase_create("check_csr_matrix_sum_100x100_zero_and_nonzero");
    tcase_add_unchecked_fixture(tc, setup_csr_matrix_100x100, teardown_matrix);
    tcase_add_unchecked_fixture(tc, setup_aug_csr_matrix_100x100, teardown_aug);
    tcase_add_test(tc, test_matrix_sum_100x100_zero_and_nonzero);
    return tc;
}

TCase *check_csr_matrix_sum_100x100_nonzero_and_zero(void) {
    TCase* tc = tcase_create("check_csr_matrix_sum_100x100_nonzero_and_zero");
    tcase_add_unchecked_fixture(tc, setup_csr_matrix_100x100, teardown_matrix);
    tcase_add_unchecked_fixture(tc, setup_aug_csr_matrix_100x100, teardown_aug);
    tcase_add_test(tc, test_matrix_sum_100x100_nonzero_and_zero);
    return tc;
}

TCase *check_csr_matrix_sum_3x3_common(void) {
    TCase* tc = tcase_create("check_csr_matrix_sum_3x3_common");
    tcase_add_unchecked_fixture(tc, setup_csr_matrix_3x3, teardown_matrix);
    tcase_add_unchecked_fixture(tc, setup_aug_csr_matrix_3x3, teardown_aug);
    tcase_add_test(tc, test_matrix_sum_3x3_common);
    return tc;
}

TCase *check_csr_matrix_sum_3x3_zero_result(void) {
    TCase* tc = tcase_create("check_csr_matrix_sum_3x3_zero_result");
    tcase_add_unchecked_fixture(tc, setup_csr_matrix_3x3, teardown_matrix);
    tcase_add_unchecked_fixture(tc, setup_aug_csr_matrix_3x3, teardown_aug);
    tcase_add_test(tc, test_matrix_sum_3x3_zero_result);
    return tc;
}

TCase *check_csr_matrix_sum_invalid_sizes(void) {
    TCase* tc = tcase_create("check_csr_matrix_sum_invalid_sizes");
    tcase_add_unchecked_fixture(tc, setup_csr_matrix_3x3, teardown_matrix);
    tcase_add_unchecked_fixture(tc, setup_aug_csr_matrix_100x100, teardown_aug);
    tcase_add_test(tc, test_matrix_sum_invalid_sizes);
    return tc;
}

Suite *check_csr_matrix_suite(void) {
    Suite* suite = suite_create("check_csr_matrix_suite");
    suite_add_tcase(suite, check_csr_matrix_invalid_0x0_size());
    suite_add_tcase(suite, check_csr_matrix_invalid_0x1_size());
    suite_add_tcase(suite, check_csr_matrix_invalid_1x0_size());
    suite_add_tcase(suite, check_csr_matrix_create_1x1());
    suite_add_tcase(suite, check_csr_matrix_create_1x2());
    suite_add_tcase(suite, check_csr_matrix_sum_100x100_zero_and_zero());
    suite_add_tcase(suite, check_csr_matrix_sum_100x100_nonzero_and_zero());
    suite_add_tcase(suite, check_csr_matrix_sum_100x100_zero_and_nonzero());
    suite_add_tcase(suite, check_csr_matrix_sum_3x3_common());
    suite_add_tcase(suite, check_csr_matrix_sum_invalid_sizes());
    suite_add_tcase(suite, check_csr_matrix_sum_3x3_zero_result());
    return suite;
}
