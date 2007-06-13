# Makefile for 4digits
all: 4digits-text

CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -g
SHELL = /bin/sh

4digits-text: 4digits-text.c
	$(CC) $(CFLAGS) 4digits-text.c -o 4digits-text

install: 4digits.6 4digits.glade 4digits_logo.png 4digits_logo.xcf 4digits-text 4digits doc/ COPYING
	install -d /usr/share/man/man6/ /usr/share/4digits/ /usr/games/ /usr/share/doc/4digits/ /usr/share/doc/4digits/images/
	install 4digits.6 /usr/share/man/man6/
	install 4digits.glade 4digits_logo.png 4digits_logo.xcf /usr/share/4digits/
	install 4digits-text 4digits /usr/games/
	install COPYING /usr/share/doc/4digits/
	cp -af doc/* /usr/share/doc/4digits/
clean:
	rm -rf 4digits-text
