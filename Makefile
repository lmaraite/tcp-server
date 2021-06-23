CC=gcc
IDIR=./include
CFLAGS=-I$(IDIR)

SRC_DIR=src
ODIR=obj

_DEPS=greeter.h utils.h clientSession.h datenhaltung.h applicationLayer.h configuration.h server.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o greeter.o utils.o datenhaltung.o applicationLayer.o configuration.o clientSession.o server.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

tcpServer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -r tcpServer $(TEST_ODIR) $(ODIR) $(TEST_OUTPUT_DIR)

# TESTING
TEST_DIR=test
TEST_ODIR=obj/test
TEST_OUTPUT_DIR=bin

TEST_FLAGS=-I./modules/cmocka/include
TEST_LIBS=-L ./modules/cmocka/lib -lcmocka

_TESTS=hello_world utils configuration clientSession
TESTS=$(patsubst %,%_test,$(_TESTS))

IDIR=./include
CFLAGS=-I$(IDIR)

.PHONY: test

test: $(TESTS)
	@echo "Executing all tests"

$(TEST_ODIR)/%.o: $(TEST_DIR)/%.c $(DEPS)
	mkdir -p $(TEST_ODIR)
	$(CC) -c -o $@ $< $(CFLAGS) $(TEST_FLAGS)

$(TEST_OUTPUT_DIR)/hello_world_test: $(TEST_ODIR)/hello_world_test.o $(ODIR)/greeter.o
	mkdir -p $(TEST_OUTPUT_DIR)
	$(CC) -o $@ $^ $(CFLAGS) $(TEST_LIBS)

$(TEST_OUTPUT_DIR)/utils_test: $(TEST_ODIR)/utils_test.o $(ODIR)/utils.o
	mkdir -p $(TEST_OUTPUT_DIR)
	$(CC) -o $@ $^ $(CFLAGS) $(TEST_LIBS)

$(TEST_OUTPUT_DIR)/configuration_test: $(TEST_ODIR)/configuration_test.o $(ODIR)/configuration.o
	mkdir -p $(TEST_OUTPUT_DIR)
	$(CC) -o $@ $^ $(CFLAGS) $(TEST_LIBS)

$(TEST_OUTPUT_DIR)/clientSession_test: $(TEST_ODIR)/clientSession_test.o $(ODIR)/clientSession.o
	mkdir -p $(TEST_OUTPUT_DIR)
	$(CC) -o $@ $^ $(CFLAGS) -Wl,--wrap=recv,--wrap=close,--wrap=send,--wrap=parseStringToCommand,--wrap=executeCommand $(TEST_LIBS)

.PHONY: hello_world_test utils_test configuration_test clientSession_test

hello_world_test: $(TEST_OUTPUT_DIR)/hello_world_test
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:./modules/cmocka/lib  \
	./$(TEST_OUTPUT_DIR)/hello_world_test

utils_test: $(TEST_OUTPUT_DIR)/utils_test
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:./modules/cmocka/lib  \
	./$(TEST_OUTPUT_DIR)/utils_test

configuration_test: $(TEST_OUTPUT_DIR)/configuration_test
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:./modules/cmocka/lib  \
	./$(TEST_OUTPUT_DIR)/configuration_test

clientSession_test: $(TEST_OUTPUT_DIR)/clientSession_test
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:./modules/cmocka/lib  \
	./$(TEST_OUTPUT_DIR)/clientSession_test
