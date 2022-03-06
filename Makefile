SHELL := /bin/bash

CC = gcc-6
CFLAGS = -O3 -std=c99 -D _POSIX_C_SOURCE=199309L -D _DEFAULT_SOURCE

default: main

main: main.c contactentry.c linkedlist.c macaddress.h
	$(CC) $(CFLAGS) -o $@ $^
