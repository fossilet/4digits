# Makefile for 4digits
all: 4digits-text

CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -g
SHELL = /bin/sh

4digits-text: 4digits-text.c
	$(CC) $(CFLAGS) 4digits-text.c -o 4digits-text
clean:
	rm -f 4digits-text
