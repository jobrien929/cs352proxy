CC = gcc
CFLAGS = -Wall -g

all: cs352proxy

.PHONY: check-syntax

check-syntax:
	$(CC) $(CFLAGS) -fsyntax-only $(CHK_SOURCES)

cs352proxy: cs352proxy.c
	$(CC) $(CFLAGS) -o cs352proxy cs352proxy.c

