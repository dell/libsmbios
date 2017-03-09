#!/bin/bash

# run this script to create all the autotools fluff. It will also run configure
# unless told not to

set -e

CURDIR=$(pwd)
SCRIPT_DIR=$(cd $(dirname $0); pwd)

cd $SCRIPT_DIR

autoreconf -v -i -f

run_configure=true
for arg in $*; do
    case $arg in
        --no-configure)
            run_configure=false
            ;;
        *)
            ;;
    esac
done

if test $run_configure = true; then
    cd $CURDIR
    $SCRIPT_DIR/configure "$@"
fi

