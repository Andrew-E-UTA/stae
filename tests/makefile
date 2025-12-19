CC:= gcc
CCFLAGS:= -O0 -g -std=c99

.phony:= tests

tests: dyn_test list_test

dyn_test: dynTest.c dyn.h simplify.h
	$(CC) $(CCFLAGS) dynTest.c -o DYN

list_test: listTest.c list.h simplify.h
	$(CC) $(CCFLAGS) listTest.c -o LIST