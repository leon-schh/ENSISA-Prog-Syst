CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g

all: ex10_1 ex10_2 ex10_3 ex10_4 ex10_5

ex10_1: ex10_1.c
	$(CC) $(CFLAGS) -o $@ $<

ex10_2: ex10_2.c
	$(CC) $(CFLAGS) -o $@ $<

ex10_3: ex10_3.c
	$(CC) $(CFLAGS) -o $@ $<

ex10_4: ex10_4.c
	$(CC) $(CFLAGS) -o $@ $<

ex10_5: ex10_5.c
	$(CC) $(CFLAGS) -o $@ $<

test: all
	@echo "=== Testing Ex10.1 (Type sizes) ==="
	./ex10_1
	@echo ""
	@echo "=== Testing Ex10.2 (Relative addresses) ==="
	./ex10_2
	@echo ""
	@echo "=== Testing Ex10.3 (Canary system) ==="
	./ex10_3
	@echo ""
	@echo "=== Testing Ex10.4 (mmap) ==="
	@echo "Test file for mmap:" > test_file.txt
	@echo "First line of test file" >> test_file.txt
	@echo "Middle line 1" >> test_file.txt
	@echo "Middle line 2" >> test_file.txt
	@echo "Last line of test file" >> test_file.txt
	./ex10_4 test_file.txt
	@echo ""
	@echo "=== Testing Ex10.5 (Custom malloc/free) ==="
	./ex10_5

clean:
	rm -f ex10_* test_file.txt

.PHONY: all test clean