TEST_FILE=test-4bit

4bit-cli: 4bit-cli.c
	@rm -f 4bit-cli;
	gcc -Wall 4bit-cli.c -o 4bit-cli

test-run:
	@rm -f $(TEST_FILE)
	gcc -Wall test-4bit.c -o $(TEST_FILE)
	./$(TEST_FILE)
	rm -f $(TEST_FILE)

test-cli: 4bit-cli
	bash test.sh
