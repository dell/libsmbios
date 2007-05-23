#!/bin/sh
# vim:et:ai:ts=4:sw=4:filetype=sh:

# the purpose of this script is to hook into git-autobuilder. It is
# called by autobuild-hook.sh as a host-specific builder. It builds RPMS
# and places them into plague.

set -e
set -x

PLAGUE_BUILDS="fc5 fc6 fcdev rhel3 rhel4 rhel5 sles9 sles10"
PREFIX=testing_

cur_dir=$(cd $(dirname $0); pwd)
cd $cur_dir/..

make -e tarball srpm

for file in ./*.src.rpm
do
	for distro in $PLAGUE_BUILDS
	do
		plague-client build $file ${PREFIX}${distro}
		sleep 5
	done
    rm $file
done
