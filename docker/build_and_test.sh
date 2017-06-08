#!/bin/sh
set -e

./autogen.sh --enable-libsmbios_cxx
make
make check
make install
