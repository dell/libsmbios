#!/bin/sh
# vim:et:ai:ts=4:sw=4:filetype=sh:

set -x

cur_dir=$(cd $(dirname $0); pwd)
cd $cur_dir/../

umask 002

[ -n "$LIBSMBIOS_TOPDIR" ] ||
    LIBSMBIOS_TOPDIR=/var/ftp/pub/Applications/libsmbios/

RELEASE_VERSION=@RELEASE_VERSION@
RELEASE_STRING=@RELEASE_STRING@
DEST=$LIBSMBIOS_TOPDIR/download/${RELEASE_NAME}/$RELEASE_STRING/

set -e

git tag -u libsmbios -m "tag for official release: $RELEASE_STRING" v${RELEASE_VERSION}

mkdir _builddir
cd _builddir
../configure
make -e srpm

mkdir -p $DEST
for i in *.tar.{gz,bz2} *.zip *.src.rpm; do
    [ -e $i ] || continue
    [ ! -e $DEST/$(basename $i) ] || continue
    cp $i $DEST
done

/var/ftp/pub/yum/dell-repo/software/_tools/upload_rpm.sh ./${RELEASE_STRING}-1.src.rpm
