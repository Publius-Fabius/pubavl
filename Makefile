CFLAGS = -g -std=c99 -pedantic -Wconversion -Wall -I include
CC = gcc

avl.o: source/pubavl/avl.c include/pubavl/avl.h
	$(CC) $(CFLAGS) -c -o $@ $<

test_avl: source/pubavl/test_avl.c avl.o 
	$(CC) $(CFLAGS) -o $@ $^

grind_test_avl: test_avl
	valgrind -q --error-exitcode=1 --leak-check=full ./$^

lib/libpubavl.a : avl.o
	ar -crs $@ $^

clean:
	rm avl.o || true
	rm test_avl || true