# Makefile for 4digits

CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -g
SHELL = /bin/sh

all: 4digits-text

4digits-text: 4digits-text.c
	$(CC) $(CFLAGS) 4digits-text.c -o 4digits-text

clean:
	rm -f 4digits-text

# Dirty hack: xgettext cannot guess that '4digits' is a python since it
# has no .py extension so we make a temporary symlink.
update-po:
	ln -s 4digits 4digits.py
	xgettext 4digits.py 4digits-text.c 4digits.glade -o po/4digits.pot -k_
	rm 4digits.py

