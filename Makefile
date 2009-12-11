TEST_FILE=test-4bit

test-run:
	@rm -f $(TEST_FILE)
	gcc -Wall test-4bit.c -o $(TEST_FILE)
	./$(TEST_FILE)
	rm -f $(TEST_FILE)
