#include <check.h>
#include <stdlib.h>

#include "check_vec.h"

int main(void) {
    SRunner *runner = srunner_create(NULL);
    srunner_add_suite(runner, check_vec_suite());

    srunner_run_all(runner, CK_NORMAL);
    const int number_failed = srunner_ntests_failed(runner);
    srunner_free(runner);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
