CC=gcc
IDIR=./include
CFLAGS=-I$(IDIR)

SRC_DIR=src
ODIR=obj

_DEPS=greeter.h utils.h applicationLayer.h
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o greeter.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRC_DIR)/%.c $(DEPS)
	mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

tcpServer: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -r tcpServer $(TEST_ODIR) $(ODIR) $(TEST_OUTPUT_DIR)

TEST_DIR=test
TEST_ODIR=obj/test
TEST_OUTPUT_DIR=bin

TEST_FLAGS=-I./modules/cmocka/include
TEST_LIBS=-L ./modules/cmocka/lib -lcmocka

_TESTS=hello_world
TESTS=$(patsubst %,%_test,$(_TESTS))

IDIR=./include
CFLAGS=-I$(IDIR)

.PHONY: test

test: $(TESTS)
	echo "Executing all tests"

$(TEST_ODIR)/%.o: $(TEST_DIR)/%.c $(DEPS)
	mkdir -p $(TEST_ODIR)
	$(CC) -c -o $@ $< $(CFLAGS) $(TEST_FLAGS)

$(TEST_OUTPUT_DIR)/hello_world_test: $(TEST_ODIR)/hello_world_test.o $(ODIR)/greeter.o
	mkdir -p $(TEST_OUTPUT_DIR)
	$(CC) -o $@ $^ $(CFLAGS) $(TEST_LIBS)

.PHONY: hello_world_test

hello_world_test: $(TEST_OUTPUT_DIR)/hello_world_test
	LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:./modules/cmocka/lib  \
	./$(TEST_OUTPUT_DIR)/hello_world_test
