PROGS=main
CC=gcc
CFLAGS=-Wall -g -std=c99 -D_POSIX_SOURCE
all: ${PROGS}

clean:
	rm -f ${PROGS} *~
