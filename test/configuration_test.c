#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>

#include "../include/configuration.h"
#include "../modules/cmocka/include/cmocka.h"
#include "logger.h"

static void resetConfig() {
    strcpy(config.PATH, "./data");
    config.PORT = 5678;
    config.MAX_SESSIONS = 5;
}

void configuration_basic_test(void **state) {
    resetConfig();
    loadConfig("./test/res/test-config");
    assert_int_equal(config.PORT, 1234);
    assert_string_equal(config.PATH, "./database/");
    assert_int_equal(config.MAX_SESSIONS, 10);
    assert_int_equal(config.LOGGING_LEVEL, WARN);
}

void configuration_append_slash_to_path_test(void **state) {
    resetConfig();
    loadConfig("./test/res/config-no-slash");
    assert_int_equal(config.PORT, 1234);
    assert_string_equal(config.PATH, "./database/");
    assert_int_equal(config.MAX_SESSIONS, 10);
}

static void configuration_failed_reading_test(void **state) {
    resetConfig();
    int error_code = loadConfig("./foo/not_existing_path.bar");
    assert_int_equal(error_code, 1001);
    assert_int_equal(config.PORT, 5678);
    assert_string_equal(config.PATH, "./data");
    assert_int_equal(config.MAX_SESSIONS, 5);
}

int main(void) {
	    const struct CMUnitTest tests[] = {
		            cmocka_unit_test(configuration_basic_test),
                    cmocka_unit_test(configuration_append_slash_to_path_test),
                    cmocka_unit_test(configuration_failed_reading_test),
			        };
	        return cmocka_run_group_tests(tests, NULL, NULL);
}
