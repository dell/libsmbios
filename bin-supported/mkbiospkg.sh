#!/bin/sh

set -e

binary=/usr/sbin/dellBiosUpdate
pkgHeader=pkgheader.sh
biosHdr=$1
output=pkg.bin

usage()
{
    echo "mkbiospkg -o OUTPUT_FILE -b BIOS.HDR -p PACKAGE_HEADER"
    echo
}

while getopts "o:b:p:h" Option
do
  case $Option in
      o)
        output=$OPTARG
        ;;
      b)
        biosHdr=$OPTARG
        ;;
      p)
        pkgHeader=$OPTARG
        ;;
      *) 
        usage
        ;;
  esac
done
shift $(($OPTIND - 1))
# Move argument pointer to next.

if [ ! -e "$biosHdr" ]; then
    echo "Require BIOS.HDR to create package."
    exit 1
fi

if [ ! -e "$pkgHeader" ]; then
    echo "Require package header to create package."
    exit 1
fi

libs=$( ldd $binary | grep '=>' | cut -d'>' -f2 | perl -p -i -e 's/\(.*\)//;' )
tmpdir=$(mktemp -d /tmp/mkpkg-XXXXXX)
tempTgz=$(mktemp /tmp/mkpkg-tgz-XXXXXX)
trap 'rm -rf $tmpdir' HUP EXIT QUIT TERM

for file in $libs $binary;
do
    rpm=$(rpm -qf --qf '%{name}-%{version}-%{release}.%{arch}' $file)
    srpm=$(rpm -qi $rpm | grep "Source RPM" | cut -d: -f3 )
    echo "$file  ==>  $srpm" >> $tmpdir/file-srpm-sources.txt
    echo $srpm >> $tmpdir/srpms.txt
done

echo "======================"
echo "SRPM List:"
cat $tmpdir/srpms.txt | sort | uniq > $tmpdir/srpms.txt2
mv $tmpdir/srpms.txt2 $tmpdir/srpms.txt
cat $tmpdir/srpms.txt
echo "======================"
echo 

cp $binary $libs $tmpdir/
cp $biosHdr $tmpdir/bios.hdr

tar czf $tempTgz -C $tmpdir .

cp $pkgHeader $output
cat $tempTgz >> $output

echo "Successfully Created $output"
echo

