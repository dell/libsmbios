#!/bin/sh
set -e

test_package () {
	if ! which $1 >/dev/null; then
		echo "Missing dpkg-checkbuilddeps, installing"
		apt install --no-install-recommends -y $2
	fi
}

test_package dpkg-checkbuilddeps dpkg-dev
test_package lsb_release lsb-release
test_package dch devscripts

DEPS=$(dpkg-checkbuilddeps pkg/debian/control 2>&1 || true)
if [ -n "$DEPS" ]; then
	echo "$DEPS, installing"
	apt install -y `echo $DEPS | sed 's,.*dependencies:,,; s,([^)]*),,'`
fi

./autogen.sh
make debs
