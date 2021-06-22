#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>

#include "utils.h"
#include "applicationLayer.h"
#include "clientSession.h"

ssize_t __wrap_recv(int fildes, void *buf, size_t nbyte, int flags) {
    check_expected(fildes);
    check_expected(nbyte);

    char* mockedValue = mock_ptr_type(char *);
    strncpy(buf, mockedValue, nbyte);
    return mock();
}

int __wrap_close(int fd) {
    check_expected(fd);
    return mock();
}

ssize_t __wrap_send(int sockfd, const void *buf, size_t len, int flags) {
    check_expected(sockfd);
    check_expected_ptr(buf);
    check_expected(len);
    check_expected(flags);
    return mock();
}

Command __wrap_parseStringToCommand(char *commandAsString) {
    check_expected_ptr(commandAsString);
    return (Command) *mock_ptr_type(Command *);
}

Result __wrap_executeCommand(Command command) {
    check_expected_ptr(&command);
    return (Result) *mock_ptr_type(Result *);
}

static void initQuitMock(int expectedFileDescriptor) {
    char expectedCommandAsString[] = "QUIT";
    Command expectedCommand;
    strcpy(expectedCommand.order, expectedCommandAsString);
    
    expect_value(__wrap_recv, fildes, expectedFileDescriptor);
    expect_value(__wrap_recv, nbyte, STRING_LENGTH*3 +1);
    will_return(__wrap_recv, expectedCommandAsString);
    will_return(__wrap_recv, 5);

    expect_string(__wrap_parseStringToCommand, commandAsString, expectedCommandAsString);
    will_return(__wrap_parseStringToCommand, &expectedCommand);

    expect_value(__wrap_close, fd, expectedFileDescriptor);
    will_return(__wrap_close, 0);
}

static void testHandleClientShouldQuit(void **state) {
    // given
    int expectedFileDescriptor = 10;
    initQuitMock(expectedFileDescriptor);
    // when
    int result = handleClient(expectedFileDescriptor);
    // then
    assert_int_equal(result, 0);
}

static void testHandleClientShouldForwardCommandToApplicationLayer(void **state) {
    // given
    int expectedFileDescriptor = 10;
    char expectedCommandAsString[] = "PUT hello world";
    Command expectedCommand = {
        .order="PUT",
        .key="hello",
        .value="world"
    };
    char *expectedValue = malloc(5);
    strcpy(expectedValue, "yes");
    
    expect_value(__wrap_recv, fildes, expectedFileDescriptor);
    expect_value(__wrap_recv, nbyte, STRING_LENGTH*3 +1);
    will_return(__wrap_recv, expectedCommandAsString);
    will_return(__wrap_recv, strlen(expectedCommandAsString) +1);

    expect_string(__wrap_parseStringToCommand, commandAsString, expectedCommandAsString);
    will_return(__wrap_parseStringToCommand, &expectedCommand);

    expect_memory(__wrap_executeCommand, &command, &expectedCommand, sizeof(Command));
    will_return(__wrap_executeCommand, &((Result) {
        .value=expectedValue,
        .error_code=0
    }));

    expect_value(__wrap_send, sockfd, expectedFileDescriptor);
    expect_string(__wrap_send, buf, "> yes\n");
    expect_value(__wrap_send, len, 7);
    expect_value(__wrap_send, flags, 0);
    will_return(__wrap_send, 4);

    initQuitMock(expectedFileDescriptor);
    // when
    int result = handleClient(expectedFileDescriptor);
    // then
    assert_int_equal(result, 0);
}

static void testHandleClientShouldSendErrorValue(void **state) {
    // given
    int expectedFileDescriptor = 10;
    char expectedCommandAsString[] = "PUT hello world";
    Command expectedCommand = {
        .order="PUT",
        .key="hello",
        .value="world"
    };
    char* expectedValue = malloc(12);
    strcpy(expectedValue, "fatal error");
    
    expect_value(__wrap_recv, fildes, expectedFileDescriptor);
    expect_value(__wrap_recv, nbyte, STRING_LENGTH*3 +1);
    will_return(__wrap_recv, expectedCommandAsString);
    will_return(__wrap_recv, strlen(expectedCommandAsString) +1);

    expect_string(__wrap_parseStringToCommand, commandAsString, expectedCommandAsString);
    will_return(__wrap_parseStringToCommand, &expectedCommand);

    expect_memory(__wrap_executeCommand, &command, &expectedCommand, sizeof(Command));
    will_return(__wrap_executeCommand, &((Result) {
        .value=expectedValue,
        .error_code=42
    }));

    expect_value(__wrap_send, sockfd, expectedFileDescriptor);
    expect_string(__wrap_send, buf, "> ERROR: fatal error\n");
    expect_value(__wrap_send, len, 22);
    expect_value(__wrap_send, flags, 0);
    will_return(__wrap_send, 20);

    initQuitMock(expectedFileDescriptor);
    // when
    int result = handleClient(expectedFileDescriptor);
    // then
    assert_int_equal(result, 0);
}

static void testHandleClientShouldSendMESSAGE_TOO_LONGWhenMessageIsTooLong(void **state) {
    // given
    int expectedFileDescriptor = 10;
    char expectedCommandAsString[128 * 4];
    for(int i = 0; i < 128 * 4; i++) {
        expectedCommandAsString[i] = 'a';
    }
    expectedCommandAsString[128 * 4 - 1] = '\0';
    expect_value(__wrap_recv, fildes, expectedFileDescriptor);
    expect_value(__wrap_recv, nbyte, STRING_LENGTH * 3 + 1);
    will_return(__wrap_recv, expectedCommandAsString);
    will_return(__wrap_recv, STRING_LENGTH * 4 +1);

    expect_value(__wrap_send, sockfd, expectedFileDescriptor);
    expect_string(__wrap_send, buf, "> ERROR: message too long\n");
    expect_value(__wrap_send, len, 24);
    expect_value(__wrap_send, flags, 0);
    will_return(__wrap_send, 24);

    initQuitMock(expectedFileDescriptor);
    // when
    int result = handleClient(expectedFileDescriptor);
    // then
    assert_int_equal(result, 0);
}

static void testHandleClientShouldReturnANY_SOCKET_EXCEPTIONWhenRecvReturnedError() {
    // given
    int expectedFileDescriptor = 10;
    
    expect_value(__wrap_recv, fildes, expectedFileDescriptor);
    expect_value(__wrap_recv, nbyte, STRING_LENGTH*3 +1);
    will_return(__wrap_recv, "");
    will_return(__wrap_recv, -1);
    // when
    int result = handleClient(expectedFileDescriptor);
    // then
    assert_int_equal(result, ANY_SOCKET_EXCEPTION);
}

static void testHandleClientShouldReturnANY_SOCKET_EXCEPTIONWhenSendReturnedError() {
    // given
    int expectedFileDescriptor = 10;
    char expectedCommandAsString[] = "PUT hello world";
    Command expectedCommand = {
        .order="PUT",
        .key="hello",
        .value="world"
    };
    char* expectedValue = malloc(4);
    strcpy(expectedValue, "yes");
    
    expect_value(__wrap_recv, fildes, expectedFileDescriptor);
    expect_value(__wrap_recv, nbyte, STRING_LENGTH*3 +1);
    will_return(__wrap_recv, expectedCommandAsString);
    will_return(__wrap_recv, strlen(expectedCommandAsString) +1);

    expect_string(__wrap_parseStringToCommand, commandAsString, expectedCommandAsString);
    will_return(__wrap_parseStringToCommand, &expectedCommand);

    expect_memory(__wrap_executeCommand, &command, &expectedCommand, sizeof(Command));
    will_return(__wrap_executeCommand, &((Result) {
        .value=expectedValue,
        .error_code=0
    }));

    expect_value(__wrap_send, sockfd, expectedFileDescriptor);
    expect_string(__wrap_send, buf, "> yes\n");
    expect_value(__wrap_send, len, 7);
    expect_value(__wrap_send, flags, 0);
    will_return(__wrap_send, -1);
    // when
    int result = handleClient(expectedFileDescriptor);
    // then
    assert_int_equal(result, ANY_SOCKET_EXCEPTION);
}

static void testHandleClientShouldReturnANY_SOCKET_EXCEPTIONWhenCloseReturnedError() {
    //given
    int expectedFileDescriptor = 10;
    char expectedCommandAsString[STRING_LENGTH] = "QUIT";
    Command expectedCommand;
    strcpy(expectedCommand.order, expectedCommandAsString);
    
    expect_value(__wrap_recv, fildes, expectedFileDescriptor);
    expect_value(__wrap_recv, nbyte, STRING_LENGTH*3 +1);
    will_return(__wrap_recv, expectedCommandAsString);
    will_return(__wrap_recv, 5);

    expect_string(__wrap_parseStringToCommand, commandAsString, expectedCommandAsString);
    will_return(__wrap_parseStringToCommand, &expectedCommand);

    expect_value(__wrap_close, fd, expectedFileDescriptor);
    will_return(__wrap_close, -1);
    //when
    int result = handleClient(expectedFileDescriptor);
    //then
    assert_int_equal(result, ANY_SOCKET_EXCEPTION);
}


int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(testHandleClientShouldQuit),
        cmocka_unit_test(testHandleClientShouldForwardCommandToApplicationLayer),
        cmocka_unit_test(testHandleClientShouldSendErrorValue),
        cmocka_unit_test(testHandleClientShouldSendMESSAGE_TOO_LONGWhenMessageIsTooLong),
        cmocka_unit_test(testHandleClientShouldReturnANY_SOCKET_EXCEPTIONWhenRecvReturnedError),
        cmocka_unit_test(testHandleClientShouldReturnANY_SOCKET_EXCEPTIONWhenSendReturnedError),
        cmocka_unit_test(testHandleClientShouldReturnANY_SOCKET_EXCEPTIONWhenCloseReturnedError)
    };
    return cmocka_run_group_tests_name("handleClient", tests, NULL, NULL);
}
