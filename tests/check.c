#include <check.h>
#include <stdlib.h>

#include "check_vec.h"
#include "check_slist.h"
#include "check_dlist.h"

int main(void) {
    SRunner *runner = srunner_create(NULL);
    srunner_add_suite(runner, check_vec_suite());
    srunner_add_suite(runner, check_slist_suite());
    srunner_add_suite(runner, check_dlist_suite());

    srunner_run_all(runner, CK_NORMAL);
    const int number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
