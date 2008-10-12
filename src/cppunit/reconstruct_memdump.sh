#!/bin/sh

# put humpty dumpty back together again...

SCRIPT_DIR=$(cd $(dirname $0); pwd)

target=$1

# start with smbios, since it should be 0xE0000
if [ -e $target/smbios.dat ]; then
    echo "laying down smbios table."
    dd if=$target/smbios.dat of=$target/memdump.dat bs=1 seek=$(( 0xE0000 )) > /dev/null 2>&1
fi

if [ -e $target/sysstr.dat ]; then
    echo "laying down system string"
    dd if=$target/sysstr.dat of=$target/memdump.dat bs=1 conv=notrunc seek=$(( 0xFE076 )) > /dev/null 2>&1
fi

if [ -e $target/idbyte.dat ]; then
    echo "laying down id byte"
    dd if=$target/idbyte.dat of=$target/memdump.dat bs=1 conv=notrunc seek=$(( 0xFE840 )) > /dev/null 2>&1
fi

echo "padding"
dd if=/dev/zero of=$target/memdump.dat bs=1 count=1 conv=notrunc seek=$(( 0x100000 - 1 )) > /dev/null 2>&1



