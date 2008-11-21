#!/bin/sh

# run this script to create all the autotools fluff

set -e

SCRIPT_DIR=$(cd $(dirname $0); pwd)
cd $SCRIPT_DIR

ACLOCALOPTS=
if grep -q ^AM_GNU_GETTEXT configure.ac 2>/dev/null; then
    cp configure.ac configure.ac~
    cp Makefile.am  Makefile.am~
    autopoint --force # gettextize replacement
    mv configure.ac~ configure.ac
    mv Makefile.am~  Makefile.am
    ACLOCALOPTS="$ACLOCALOPTS -I m4"
fi

aclocal $ACLOCALOPTS

if grep -q ^AC_PROG_LIBTOOL configure.ac 2>/dev/null; then
    libtoolize -c --force --automake
fi

if grep -q ^AC_CONFIG_HEADER configure.ac 2>/dev/null; then
    autoheader --force
fi

automake --force --foreign --add-missing -c
autoconf --force
