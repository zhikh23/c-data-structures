#include <check.h>
#include <stdlib.h>

#include "check_vec.h"
#include "check_slist.h"
#include "check_dlist.h"
#include "check_maps.h"
#include "check_stacks.h"
#include "check_matrices.h"


int main(void) {
    SRunner *runner = srunner_create(NULL);
    srunner_add_suite(runner, check_vec_suite());
    srunner_add_suite(runner, check_slist_suite());
    srunner_add_suite(runner, check_dlist_suite());
    srunner_add_suite(runner, check_bstree_suite());
    srunner_add_suite(runner, check_avltree_suite());
    srunner_add_suite(runner, check_shmap_suite());
    srunner_add_suite(runner, check_hmap_suite());
    srunner_add_suite(runner, check_astack_suite());
    srunner_add_suite(runner, check_lstack_suite());
    srunner_add_suite(runner, check_flat_matrix_suite());
    srunner_add_suite(runner, check_csr_matrix_suite());

    srunner_run_all(runner, CK_NORMAL);
    const int number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
