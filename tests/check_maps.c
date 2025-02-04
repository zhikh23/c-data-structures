#include "check_maps.h"

#include "avltree.h"
#include "bstree.h"
#include "hash.h"
#include "hmap.h"
#include "map.h"
#include "shmap.h"


#define ck_assert_true(x) ck_assert_int_eq(!!(x), 1)
#define ck_assert_false(x) ck_assert_int_eq(!!(x), 0)

#define LEN(arr) (sizeof(arr) / sizeof(arr[0]))


static void *map;


static void setup_bstree(void) {
    map = map_new(BinarySearchTree);
}

static void setup_avltree(void) {
    map = map_new(AVLTree);
}

static void setup_shmap(void) {
    map = map_new(SimpleHashMap, 0, djb2);
}

static void setup_hmap(void) {
    map = map_new(HashMap, djb2);
}

static void teardown_map(void) {
    map_destroy(map);
    map = NULL;
}


START_TEST (test_map_insert_and_lookup) {
    map_insert(map, "foo", 2);
    const map_res_t res = map_lookup(map, "foo");
    ck_assert_true(res.ok);
    ck_assert_int_eq(res.data, 2);
} END_TEST

START_TEST (test_map_lookup_not_existing) {
    const map_res_t res = map_lookup(map, "foo");
    ck_assert_false(res.ok);
    ck_assert_int_eq(res.data, 0);
} END_TEST

START_TEST (test_map_insert_many_and_lookup) {
    const string_t *keys[] = { "a", "aa", "baa", "aab", "b", "baba", "ba", "ab", "bab" };
    const int       vals[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    _Static_assert(LEN(keys) == LEN(vals));

    for (size_t i = 0; i < LEN(keys); i++)
        map_insert(map, keys[i], vals[i]);

    for (size_t i = 0; i < LEN(keys); i++) {
        const map_res_t res = map_lookup(map, keys[i]);
        ck_assert_true(res.ok);
        ck_assert_int_eq(res.data, vals[i]);
    }
} END_TEST

START_TEST (test_map_insert_many_and_remove_all) {
    const string_t *keys[] = { "a", "aa", "baa", "aab", "b", "baba", "ba", "ab", "bab" };
    const int       vals[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    _Static_assert(LEN(keys) == LEN(vals));

    for (size_t i = 0; i < LEN(keys); i++)
        map_insert(map, keys[i], vals[i]);

    for (size_t i = 0; i < LEN(keys); i++)
        ck_assert_true(map_lookup(map, keys[i]).ok);

    for (size_t i = 0; i < LEN(keys); i++) {
        const int deleted = map_remove(map, keys[i]);
        ck_assert_true(deleted);
    }

    for (size_t i = 0; i < LEN(keys); i++)
        ck_assert_false(map_lookup(map, keys[i]).ok);

} END_TEST

START_TEST (test_map_insert_update) {
    map_insert(map, "foo", 2);
    map_insert(map, "foo", 3);
    const map_res_t res = map_lookup(map, "foo");
    ck_assert_true(res.ok);
    ck_assert_int_eq(res.data, 3);
} END_TEST

TCase *check_bstree_insert_and_lookup(void) {
    TCase *tc = tcase_create("check_bstree_insert_and_lookup");
    tcase_add_unchecked_fixture(tc, setup_bstree, teardown_map);
    tcase_add_test(tc, test_map_insert_and_lookup);
    return tc;
}

TCase *check_bstree_lookup_not_existing(void) {
    TCase *tc = tcase_create("check_bstree_lookup_not_existing");
    tcase_add_unchecked_fixture(tc, setup_bstree, teardown_map);
    tcase_add_test(tc, test_map_lookup_not_existing);
    return tc;
}

TCase *check_bstree_insert_many_and_lookup(void) {
    TCase *tc = tcase_create("check_bstree_insert_many_and_lookup");
    tcase_add_unchecked_fixture(tc, setup_bstree, teardown_map);
    tcase_add_test(tc, test_map_insert_many_and_lookup);
    return tc;
}

TCase *check_bstree_insert_many_and_remove_all(void) {
    TCase *tc = tcase_create("check_bstree_insert_many_and_remove_all");
    tcase_add_unchecked_fixture(tc, setup_bstree, teardown_map);
    tcase_add_test(tc, test_map_insert_many_and_remove_all);
    return tc;
}

TCase *check_bstree_insert_update(void) {
    TCase *tc = tcase_create("check_bstree_insert_update");
    tcase_add_unchecked_fixture(tc, setup_bstree, teardown_map);
    tcase_add_test(tc, test_map_insert_update);
    return tc;
}

Suite *check_bstree_suite(void) {
    Suite *suite = suite_create("check_bstree");
    suite_add_tcase(suite, check_bstree_insert_and_lookup());
    suite_add_tcase(suite, check_bstree_lookup_not_existing());
    suite_add_tcase(suite, check_bstree_insert_many_and_lookup());
    suite_add_tcase(suite, check_bstree_insert_many_and_remove_all());
    suite_add_tcase(suite, check_bstree_insert_update());
    return suite;
}

TCase *check_avltree_insert_and_lookup(void) {
    TCase *tc = tcase_create("check_avltree_insert_and_lookup");
    tcase_add_unchecked_fixture(tc, setup_avltree, teardown_map);
    tcase_add_test(tc, test_map_insert_and_lookup);
    return tc;
}

TCase *check_avltree_lookup_not_existing(void) {
    TCase *tc = tcase_create("check_avltree_lookup_not_existing");
    tcase_add_unchecked_fixture(tc, setup_avltree, teardown_map);
    tcase_add_test(tc, test_map_lookup_not_existing);
    return tc;
}

TCase *check_avltree_insert_many_and_lookup(void) {
    TCase *tc = tcase_create("check_avltree_insert_many_and_lookup");
    tcase_add_unchecked_fixture(tc, setup_avltree, teardown_map);
    tcase_add_test(tc, test_map_insert_many_and_lookup);
    return tc;
}

TCase *check_avltree_insert_many_and_remove_all(void) {
    TCase *tc = tcase_create("check_avltree_insert_many_and_remove_all");
    tcase_add_unchecked_fixture(tc, setup_avltree, teardown_map);
    tcase_add_test(tc, test_map_insert_many_and_remove_all);
    return tc;
}

TCase *check_avltree_insert_update(void) {
    TCase *tc = tcase_create("check_avltree_insert_update");
    tcase_add_unchecked_fixture(tc, setup_avltree, teardown_map);
    tcase_add_test(tc, test_map_insert_update);
    return tc;
}

Suite *check_avltree_suite(void) {
    Suite *suite = suite_create("check_avltree");
    suite_add_tcase(suite, check_avltree_insert_and_lookup());
    suite_add_tcase(suite, check_avltree_lookup_not_existing());
    suite_add_tcase(suite, check_avltree_insert_many_and_lookup());
    suite_add_tcase(suite, check_avltree_insert_many_and_remove_all());
    suite_add_tcase(suite, check_avltree_insert_update());
    return suite;
}

TCase *check_shmap_insert_and_lookup(void) {
    TCase *tc = tcase_create("check_shmap_insert_and_lookup");
    tcase_add_unchecked_fixture(tc, setup_shmap, teardown_map);
    tcase_add_test(tc, test_map_insert_and_lookup);
    return tc;
}

TCase *check_shmap_lookup_not_existing(void) {
    TCase *tc = tcase_create("check_shmap_lookup_not_existing");
    tcase_add_unchecked_fixture(tc, setup_shmap, teardown_map);
    tcase_add_test(tc, test_map_lookup_not_existing);
    return tc;
}

TCase *check_shmap_insert_many_and_lookup(void) {
    TCase *tc = tcase_create("check_shmap_insert_many_and_lookup");
    tcase_add_unchecked_fixture(tc, setup_shmap, teardown_map);
    tcase_add_test(tc, test_map_insert_many_and_lookup);
    return tc;
}

TCase *check_shmap_insert_many_and_remove_all(void) {
    TCase *tc = tcase_create("check_shmap_insert_many_and_remove_all");
    tcase_add_unchecked_fixture(tc, setup_shmap, teardown_map);
    tcase_add_test(tc, test_map_insert_many_and_remove_all);
    return tc;
}

TCase *check_shmap_insert_update(void) {
    TCase *tc = tcase_create("check_shmap_insert_update");
    tcase_add_unchecked_fixture(tc, setup_shmap, teardown_map);
    tcase_add_test(tc, test_map_insert_update);
    return tc;
}

Suite *check_shmap_suite(void) {
    Suite *suite = suite_create("check_shmap");
    suite_add_tcase(suite, check_shmap_insert_and_lookup());
    suite_add_tcase(suite, check_shmap_lookup_not_existing());
    suite_add_tcase(suite, check_shmap_insert_many_and_lookup());
    suite_add_tcase(suite, check_shmap_insert_many_and_remove_all());
    suite_add_tcase(suite, check_shmap_insert_update());
    return suite;
}

TCase *check_hmap_insert_and_lookup(void) {
    TCase *tc = tcase_create("check_hmap_insert_and_lookup");
    tcase_add_unchecked_fixture(tc, setup_hmap, teardown_map);
    tcase_add_test(tc, test_map_insert_and_lookup);
    return tc;
}

TCase *check_hmap_lookup_not_existing(void) {
    TCase *tc = tcase_create("check_hmap_lookup_not_existing");
    tcase_add_unchecked_fixture(tc, setup_hmap, teardown_map);
    tcase_add_test(tc, test_map_lookup_not_existing);
    return tc;
}

TCase *check_hmap_insert_many_and_lookup(void) {
    TCase *tc = tcase_create("check_hmap_insert_many_and_lookup");
    tcase_add_unchecked_fixture(tc, setup_hmap, teardown_map);
    tcase_add_test(tc, test_map_insert_many_and_lookup);
    return tc;
}

TCase *check_hmap_insert_many_and_remove_all(void) {
    TCase *tc = tcase_create("check_hmap_insert_many_and_remove_all");
    tcase_add_unchecked_fixture(tc, setup_hmap, teardown_map);
    tcase_add_test(tc, test_map_insert_many_and_remove_all);
    return tc;
}

TCase *check_hmap_insert_update(void) {
    TCase *tc = tcase_create("check_hmap_insert_update");
    tcase_add_unchecked_fixture(tc, setup_hmap, teardown_map);
    tcase_add_test(tc, test_map_insert_update);
    return tc;
}

Suite *check_hmap_suite(void) {
    Suite *suite = suite_create("check_hmap");
    suite_add_tcase(suite, check_hmap_insert_and_lookup());
    suite_add_tcase(suite, check_hmap_lookup_not_existing());
    suite_add_tcase(suite, check_hmap_insert_many_and_lookup());
    suite_add_tcase(suite, check_hmap_insert_many_and_remove_all());
    suite_add_tcase(suite, check_hmap_insert_update());
    return suite;
}
