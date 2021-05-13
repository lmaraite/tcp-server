#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include "../include/utils.h"

static void parseStringToCommand_order_key_value_test(void **state) {
    Command command = parseStringToCommand("ORDER KEY VALUE");
    assert_string_equal(command.order, "ORDER");
    assert_string_equal(command.key, "KEY");
    assert_string_equal(command.value, "VALUE");
}

static void parseStringToCommand_order_key_test(void **state) {
    Command command = parseStringToCommand("ORDER KEY");
    assert_string_equal(command.order, "ORDER");
    assert_string_equal(command.key, "KEY");
    assert_string_equal(command.value, "");
}

static void parseStringToCommand_only_order_test(void **state) {
    Command command = parseStringToCommand("ORDER");
    assert_string_equal(command.order, "ORDER");
    assert_string_equal(command.key, "");
    assert_string_equal(command.value, "");
}

static void parseStringToCommand_empty_string_test(void **state) {
    Command command = parseStringToCommand("");
    assert_string_equal(command.order, "");
    assert_string_equal(command.key, "");
    assert_string_equal(command.value, "");
}

static void parseStringToCommand_wrong_format_test(void **state) {
    Command command = parseStringToCommand("ORDER-KEY-VALUE");
    assert_string_equal(command.order, "ORDER-KEY-VALUE");
    assert_string_equal(command.key, "");
    assert_string_equal(command.value, "");
}

static void parseStringToCommand_too_many_arguments_test(void **state) {
    Command command = parseStringToCommand("ORDER KEY VALUE TOO MUCH");
    assert_string_equal(command.order, "ORDER");
    assert_string_equal(command.key, "KEY");
    assert_string_equal(command.value, "VALUE");
}

int main(void) {
    const struct CMUnitTest tests[] = {
            cmocka_unit_test(parseStringToCommand_order_key_value_test),
            cmocka_unit_test(parseStringToCommand_order_key_test),
            cmocka_unit_test(parseStringToCommand_only_order_test),
            cmocka_unit_test(parseStringToCommand_empty_string_test),
            cmocka_unit_test(parseStringToCommand_wrong_format_test),
            cmocka_unit_test(parseStringToCommand_too_many_arguments_test),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}

