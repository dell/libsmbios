#!/bin/sh
# vim:et:ai:ts=4:sw=4:filetype=sh:tw=0:

set -x

cur_dir=$(cd $(dirname $0); pwd)
cd $cur_dir/../

umask 002

[ -n "$LIBSMBIOS_TOPDIR" ] ||
    LIBSMBIOS_TOPDIR=/var/ftp/pub/Applications/libsmbios/

set -e

chmod -R +w _builddir ||:
rm -rf _builddir

./autogen.sh

mkdir _builddir
pushd _builddir
../configure
make rpm RPM_DEFINES="--without unit-tests"
make distcheck

make git-tag
eval "$(make get-version)"

DEST=$LIBSMBIOS_TOPDIR/download/$PACKAGE/$PACKAGE-$PACKAGE_VERSION/
mkdir -p $DEST
for i in *.tar.{gz,bz2} *.zip *.src.rpm; do
    [ -e $i ] || continue
    [ ! -e $DEST/$(basename $i) ] || continue
    cp $i $DEST
done

git push --tags origin master:master
