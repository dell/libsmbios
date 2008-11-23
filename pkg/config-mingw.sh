#!/bin/sh

SCRIPT_DIR=$(cd $(dirname $0); pwd)

$SCRIPT_DIR/../configure --host=i686-pc-mingw32 --target=i686-pc-mingw32 --with-cppunit-prefix=/usr/i686-pc-mingw32/sys-root/  --with-cppunit-exec-prefix=/usr/i686-pc-mingw32/sys-root/mingw/
