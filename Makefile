TEST_FILE=test-4bit

TCDB_DIR=./libs/tinycdb
TCDB_STATIC=$(TCDB_DIR)/libcdb.a

4bit-cli: 4bit-cli.c 4bit.h $(TCDB_STATIC)
	@rm -f 4bit-cli;
	gcc -I$(TCDB_DIR) -Wall 4bit-cli.c $(TCDB_STATIC) -o 4bit-cli

$(TCDB_STATIC):
	cd $(TCDB_DIR) && make staticlib

test-run:
	@rm -f $(TEST_FILE)
	gcc -Wall test-4bit.c -o $(TEST_FILE)
	./$(TEST_FILE)
	rm -f $(TEST_FILE)

test-cli: 4bit-cli
	bash cli-test.sh
