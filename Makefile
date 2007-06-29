# Makefile for 4digits
all: 4digits-text

CC = gcc
CFLAGS = -Wall -std=c99 -pedantic -g
SHELL = /bin/sh

4digits-text: 4digits-text.c
	$(CC) $(CFLAGS) 4digits-text.c -o 4digits-text

install: #4digits.6.gz 4digits.64digits.glade 4digits_logo.png 4digits_logo.xcf 4digits-text 4digits doc/ COPYING TODO 4digits.desktop
	install -d /usr/share/man/man6/ /usr/share/4digits/ /usr/games/ /usr/share/doc/4digits/ /usr/share/pixmaps/ /usr/share/icons/hicolor/48x48/apps/
	install 4digits /usr/games/4digits
	install 4digits.glade /usr/share/4digits/4digits.glade
	install 4digits-text /usr/games/4digits-text
	install 4digits.6.gz /usr/share/man/man6/4digits.6.gz
	install 4digits-text.6.gz /usr/share/man/man6/4digits-text.6.gz
	install 4digits.desktop /usr/share/applications/4digits.desktop
	install 4digits_logo.png /usr/share/4digits/4digits_logo.png
	install 4digits_logo.png /usr/share/pixmaps/4digits_logo.png
	install 4digits_logo.png /usr/share/icons/hicolor/48x48/apps/
	install 4digits_logo.xcf /usr/share/4digits/4digits_logo.xcf
	cp -af doc/* /usr/share/doc/4digits/
clean:
	rm -rf 4digits-text