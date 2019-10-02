all:	test.c library.o
	gcc test.c library.o

library.o: library.c
	gcc -c library.c

clean:
	rm library.o; rm a.out

test0:
	./a.out
