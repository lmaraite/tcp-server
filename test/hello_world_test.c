#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "../include/greeter.h"

/* A test case that does nothing and succeeds. */
static void null_test_success(void **state) {
	    (void) state; /* unused */
}

static void greet_test(void **state) {
    char *result = greet("Leon");
    assert_string_equal("Hello, Leon!", result);
}

int main(void) {
	    const struct CMUnitTest tests[] = {
		            cmocka_unit_test(null_test_success),
                    cmocka_unit_test(greet_test),
			        };
	        return cmocka_run_group_tests(tests, NULL, NULL);
}
