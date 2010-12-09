CC=gcc --std=c99 -Iinclude

all: build/squish

build/gopt.o : src/gopt.c include/gopt.h
	mkdir -p build
	$(CC) -c -o build/gopt.o src/gopt.c

build/squish : build/gopt.o src/squish.c
	$(CC) -Wall -pedantic -Werror -o build/squish src/squish.c build/gopt.o

clean:
	rm -rf build
