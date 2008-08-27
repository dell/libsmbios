#!/bin/sh
# vim:et:ai:ts=4:sw=4:filetype=sh:tw=0:

# the purpose of this script is to hook into git-autobuilder. It is
# called by autobuild-hook.sh as a host-specific builder. It builds RPMS
# and places them into plague.

set -x

cur_dir=$(cd $(dirname $0); pwd)
cd $cur_dir/..

umask 002

set -e

./autogen.sh
mkdir _builddir
cd _builddir
../configure
make -e distcheck
make -e srpm

. version

/var/ftp/pub/yum/dell-repo/testing/_tools/upload_rpm.sh ./${PACKAGE_NAME}-${PACKAGE_VERSION}-*.src.rpm
