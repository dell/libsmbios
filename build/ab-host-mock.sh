#!/bin/sh
# vim:et:ai:ts=4:sw=4:filetype=sh:

set -e
set -x

# dont run this from radon.
PLAGUE_BUILDS="fc5 fc6 fcdev rhel3 rhel4 rhel5 sles9 sles10"
PREFIX=testing_

[ -n "$LIBSMBIOS_TOPDIR" ] ||
    LIBSMBIOS_TOPDIR=/var/ftp/pub/Applications/libsmbios/

cur_dir=$(cd $(dirname $0); pwd)
cd $cur_dir/../
. version.mk
RELEASE_VERSION=${RELEASE_MAJOR}.${RELEASE_MINOR}.${RELEASE_SUBLEVEL}${RELEASE_EXTRALEVEL}
RELEASE_STRING=${RELEASE_NAME}-${RELEASE_VERSION}

DEST=$LIBSMBIOS_TOPDIR/download/$(basename $PWD)/$RELEASE_STRING/
mkdir -p $DEST

rm -rf _buildtemp
mkdir _buildtemp
pushd _buildtemp
    ../configure
    make srpm
popd

for i in _buildtemp/*.tar.{gz,bz2} _buildtemp/*.zip _buildtemp/*.src.rpm; do
    [ -e $i ] || continue
    [ ! -e $DEST/$(basename $i) ] || continue
    cp $i $DEST
done

scp -4qr -i ~/.ssh/id_dsa_fwupdate _buildtemp/*.src.rpm autobuilder@mock.linuxdev.us.dell.com:~/queue/

for i in _buildtemp/*.src.rpm
do
	file=$(basename $i)
	for distro in $PLAGUE_BUILDS
	do
		ssh -4 -i ~/.ssh/id_dsa_fwupdate autobuilder@mock.linuxdev.us.dell.com plague-client build \~/queue/$file ${PREFIX}${distro}
		sleep 5
	done
    ssh -4 -i ~/.ssh/id_dsa_fwupdate autobuilder@mock.linuxdev.us.dell.com rm \~/queue/$file
done

#rm -rf _buildtemp

