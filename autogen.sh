#!/bin/sh

# run this script to create all the autotools fluff

set -e
aclocal
libtoolize -c --force --automake
automake --force --foreign --add-missing -c
autoconf --force
