CC=gcc
CCFLAGS=-O3 -g -Wall -pedantic -Wextra
LDFLAGS=

.PHONY: default stand-alone library clean

default: stand-alone library


stand-alone: network.x

library: libnetwork.so
	

network.x: main.o# Dinamica.o
	$(CC) $(LDFLAGS) $^ -o $@

libnetwork.so: Dinamica.o
	$(CC) $(LDFLAGS) $^ -shared -o $@

%.o: %.c
	$(CC) $(CCFLAGS) $< -c -fPIC

clean:
	rm -rf *.o *.x
