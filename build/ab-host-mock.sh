#!/bin/sh

# the purpose of this script is to hook into git-autobuilder. It is
# called by autobuild-hook.sh as a host-specific builder. It builds RPMS
# and places them into plague.

CURDIR=$(cd $(dirname $0); pwd)

cd $CURDIR/..

./configure
make check
make srpm
