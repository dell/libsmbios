#!/bin/sh

# run this script to create all the autotools fluff

set -e

SCRIPT_DIR=$(cd $(dirname $0); pwd)
cd $SCRIPT_DIR

cp configure.ac configure.ac~
cp Makefile.am  Makefile.am~
autopoint --force # gettextize replacement
mv configure.ac~ configure.ac
mv Makefile.am~  Makefile.am

aclocal -I m4
libtoolize -c --force --automake
autoheader --force
automake --force --foreign --add-missing -c
autoconf --force
