# Makefile for 4digits

CC = cc
CFLAGS = -Wall -std=c99 -pedantic -g -I/usr/local/opt/gettext/include/ -L/usr/local/opt/gettext/lib/
SHELL = /bin/sh
# For distribution packaging
#INSTALL_DIR = /usr/share/locale
INSTALL_DIR = locale

#all: 4digits-text compile-po
all: 4digits-text update-po compile-po install-po

trans: update-po compile-po install-po

4digits-text: 4digits-text.c
	$(CC) $(CFLAGS) 4digits-text.c -o 4digits-text

clean:
	rm -f 4digits-text
	rm -f po/*.mo po/*.po~

# Dirty hack: xgettext cannot guess that '4digits' is a python since it
# has no .py extension so we make a temporary symlink.
update-po:
	#Update 4digits.pot
	ln -sf 4digits 4digits.py
	xgettext 4digits.py 4digits-text.c 4digits.glade -o po/4digits.pot -k_ -kN_
	rm 4digits.py
	#Update translations
	for f in po/*.po; do \
		msgmerge -U $$f po/4digits.pot; \
	done

compile-po:
	for f in po/*.po; do \
		g=`echo $$f | sed -e 's/\.po/\.mo/'`; \
		msgfmt $$f -o $$g; \
	done

install-po:
	for g in po/*.mo; do \
		l=`echo $$g | sed -e 's#po/\(.*\)\.mo#\1#'`; \
		install -m644 -D $$g $(INSTALL_DIR)/$$l/LC_MESSAGES/4digits.mo; \
	done
