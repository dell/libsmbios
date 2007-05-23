#!/bin/sh

CURDIR=$(cd $(dirname $0); pwd)

[ -z "$BUILD_CYCLE" ] || export RELEASE_EXTRALEVEL=.${BUILD_CYCLE}.autobuild
if [ -x $CURDIR/ab-host-$(hostname)-$(id -un).sh ]; then
    $CURDIR/ab-host-$(hostname)-$(id -un).sh
elif [ -x $CURDIR/ab-host-$(hostname).sh ]; then
    $CURDIR/ab-host-$(hostname).sh
elif [ -x $CURDIR/ab-generic.sh ]; then
    $CURDIR/ab-generic.sh
fi
