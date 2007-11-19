#! /usr/bin/env python

# This file is stolen from comix. Thanks Pontus Ekberg!

import os
import sys
import getopt
import shutil

usage_info = """
This script installs or uninstalls 4digits on your system.
If you encounter any bugs, please report them to fossilet@users.sf.net.

--------------------------------------------------------------------------------

Usage:

 ./install.py install              ---      Install to /usr/local

 ./install.py uninstall            ---      Uninstall from /usr/local

--------------------------------------------------------------------------------

Options:

 --installdir <directory>          ---      Install or uninstall in <directory>
                                            instead of /usr/local
"""

def info():
    print usage_info
    sys.exit(1)

def install(src, dst):
    try:
        dst = os.path.join(install_dir, dst)
        assert os.path.isfile(src)
        assert not os.path.isdir(dst)
        if not os.path.isdir(os.path.dirname(dst)):
            os.makedirs(os.path.dirname(dst))
        shutil.copy(src, dst)
        print "Installed", dst
    except:
        print "Error while installing", src, 'to',  dst

def uninstall(path):
    try:
        path = os.path.join(install_dir, path)
        if os.path.isfile(path):
            os.remove(path)
        elif os.path.isdir(path):
            shutil.rmtree(path)
        else:
            return
        print "Removed", path
    except:
        print "Error while removing", path

def check_dependencies():
    required_found = True
    recommended_found = True
    print "Checking dependencies..."
    print
    print "Required dependencies:"
    print
    # Should also check the PyGTK version. To do that we have to load the
    # gtk module though, which normally can't be done while using `sudo`.
    try:
        import pygtk
        print "    PyGTK ........................ OK"
    except ImportError:
        print "    !!! PyGTK .................... Not found"
        required_found = False
    try:
        import gtk.glade
        print "    Python Glade ................. OK"
    except ImportError:
        print "    !!! Python Glade ............. Not found"
        required_found = False
    if not required_found:
        print
        print "Could not find all required dependencies!"
        print "Please install them and try again."
        print
        sys.exit(1)
    print

install_dir = "/usr/local/"

try:
    opts, args = \
        getopt.gnu_getopt(sys.argv[1:], "",
        ["installdir="])
except getopt.GetoptError:
    info()
for opt, value in opts:
    if opt == "--installdir":
        install_dir = value
        if not os.path.isdir(install_dir):
            print "\n*** Error:", install_dir, "does not exist.\n" 
            info()

if args == ["install"]:
    check_dependencies()
    print "Installing 4digits in", install_dir, "...\n"
    install("4digits", "games/4digits")
    install("4digits.glade", "share/4digits/4digits.glade")
    install("4digits-text", "games/4digits-text")
    install("4digits.6.gz", "share/man/man6/4digits.6.gz")
    install("4digits-text.6.gz", "share/man/man6/4digits-text.6.gz")
    install("4digits.desktop", "share/applications/4digits.desktop")
    install("4digits_logo.png", "share/4digits/4digits_logo.png")
    install("4digits_logo.png", "share/pixmaps/4digits_logo.png")
    install("4digits.xpm", "share/pixmaps/4digits.xpm")
    install("4digits_logo.png", "share/icons/hicolor/48x48/apps/4digits_logo.png")
    install("4digits_logo.xcf", "share/4digits/4digits_logo.xcf")

    for file in os.listdir('doc'):
        if os.path.isfile(os.path.join('doc', file)):
            install(os.path.join('doc', file),
                os.path.join('share/doc/4digits/', file))
    install("INSTALL", "share/doc/4digits/INSTALL")
    install("COPYING", "share/doc/4digits/COPYING")
    for file in os.listdir('doc/images/'):
        if os.path.isfile(os.path.join('doc/images/', file)):
            install(os.path.join('doc/images/', file),
                os.path.join('share/doc/4digits/images/', file))
elif args == ["uninstall"]:
    print "Uninstalling 4digits from", install_dir, "...\n"
    uninstall("games/4digits")
    uninstall("share/4digits.glade")
    uninstall("games/4digits-text")
    uninstall("share/man/man6/4digits.6.gz")
    uninstall("share/man/man6/4digits-text.6.gz")
    uninstall("share/applications/4digits.desktop")
    uninstall("share/4digits/4digits_logo.png")
    uninstall("share/pixmaps/4digits_logo.png")
    uninstall("share/pixmaps/4digits.xpm")
    uninstall("share/icons/hicolor/48x48/apps/4digits_logo.png")
    uninstall("share/4digits/4digits_logo.xcf")
    uninstall("share/4digits/")
    uninstall("share/doc/4digits/")
    print
    print "There might still be files in your $HOME/.4digits/ directory."
    print "Please remove that directory manually if you do not plan to"
    print "install 4digits again later."
    
else:
    info()
