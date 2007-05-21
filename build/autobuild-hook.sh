#!/bin/sh

CURDIR=$(cd $(dirname $0); pwd)

if [ -x $CURDIR/ab-host-$(hostname).sh ]; then
    $CURDIR/ab-host-$(hostname).sh
fi
