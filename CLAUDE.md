# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with
code in this repository.

## Build & Run Commands

```bash
# Run GUI version (requires PyGObject/Gtk3)
./4digits

# Compile and run text version
make 4digits-text
./4digits-text

# Build everything including translations
make

# Update translation template (POT file)
make update-po

# Compile translations
make compile-po

# Install translations to locale/
make install-po

# Clean compiled files
make clean
```

## Architecture

- `4digits` - Python main game script (no .py extension)
- `4digits.glade` - GTK3 UI definition (Glade XML format)
- `4digits-text.c` - C implementation of text version
- `po/` - Gettext translation source files (.po)
- `locale/` - Compiled binary translations (.mo)
- `doc/` - HTML documentation and images

The Python version uses PyGObject (gi) to access GTK3. The game stores prefs in
`~/.4digits/prefs.pickle` and scores in `~/.4digits/4digits.4digits.scores`.

## Translation Workflow

Translations are managed on Launchpad and incorporated via the `po/` directory:

1. Update POT template: `make update-po` (runs xgettext on 4digits.py,
   4digits-text.c, 4digits.glade)
2. Merge translations: `msgmerge -U po/<lang>.po po/4digits.pot`
3. Compile: `msgfmt po/<lang>.po -o po/<lang>.mo`
4. Install: copy .mo to `locale/<lang>/LC_MESSAGES/4digits.mo`

Test translations with: `LANGUAGE=zh_TW ./4digits`
