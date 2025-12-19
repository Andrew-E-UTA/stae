CC:= gcc
CCFLAGS:= -O0 -g -std=c99

.phony:= tests

tests: dyn_test list_test

debug: scratch.c
	$(CC) $(CCFLAGS) scratch.c

dyn_test: dynTest.c ../dyn.h ../simplify.h
	$(CC) $(CCFLAGS) dynTest.c -o dynTest.exe

list_test: listTest.c ../list.h ../simplify.h
	$(CC) $(CCFLAGS) listTest.c #-o listTest.exe

clean:
	rm *.exe