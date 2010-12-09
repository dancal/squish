CC=gcc --std=c99 -Iinclude

all: bin/squish

build/gopt.o: src/gopt.c include/gopt.h
	mkdir -p build
	$(CC) -c -o build/gopt.o src/gopt.c

bin/squish: build/gopt.o src/squish.c
	mkdir -p bin 
	$(CC) -Wall -pedantic -Werror -o bin/squish src/squish.c build/gopt.o

clean:
	rm -rf build bin

install: bin/squish
	install bin/squish /usr/bin/squish 
