#!/bin/sh

#set -x
set -e

DIR=$(cd $(dirname $0); pwd)

TMPDIR=$PWD/out/test
mkdir -p $TMPDIR

[ -n "$TST" ] || TST=out

export MALLOC_PERTURB_=0xBABADABA

reconstruct_memdump() {
    source_dir=$1
    target_dir=$2

    [ ! -e $target_dir/memdump.dat ] || return 0

    dd if=/dev/zero of=$target_dir/memdump.dat bs=1 count=1 conv=notrunc seek=$(( 0x100000 - 1 )) > /dev/null 2>&1

    # start with smbios, since it should be 0xE0000
    if [ -e $source_dir/smbios.dat ]; then
        #echo "laying down smbios table."
        dd if=$source_dir/smbios.dat of=$target_dir/memdump.dat bs=1 conv=notrunc seek=$(( 0xE0000 )) > /dev/null 2>&1
    fi

    if [ -e $source_dir/sysstr.dat ]; then
        #echo "laying down system string"
        dd if=$source_dir/sysstr.dat of=$target_dir/memdump.dat bs=1 conv=notrunc seek=$(( 0xFE076 )) > /dev/null 2>&1
    fi

    if [ -e $source_dir/idbyte.dat ]; then
        #echo "laying down id byte"
        dd if=$source_dir/idbyte.dat of=$target_dir/memdump.dat bs=1 conv=notrunc seek=$(( 0xFE840 )) > /dev/null 2>&1
    fi

    for offsetfn in $source_dir/offset-*.dat;
    do
        [ -e $offsetfn ] || continue
        fn=$(basename $offsetfn)
        basefn=$(basename $fn .dat)
        offset=${basefn##offset-}
        echo "lay down offsets $offset"
        dd if=$offsetfn of=$target_dir/memdump.dat bs=1 conv=notrunc seek=$(( $offset )) > /dev/null 2>&1
    done
}

run_test() {
    target_dir=$TMPDIR
    test_binary=$1
    source_dir=$2
    echo -e $3
    if [ ! -e $TST/${test_binary} -a -e $TST/${test_binary}.exe ]; then
        test_binary=${test_binary}.exe
    fi
    rm -rf $TMPDIR/*
    if [ -n "$source_dir" ]; then
        cp $source_dir/* $target_dir/ ||:
        reconstruct_memdump $target_dir $target_dir
    fi
    $VALGRIND $TST/$test_binary $TMPDIR $(basename "$source_dir")
}


[ ${RUN_C_TEST:=1} -ne 1 ] || run_test testC_memory_cmos ""                  "\n\nRunning CInterface tests."
[ ${RUN_CPP_TEST} -ne 1 ] || run_test testStandalone    $DIR/test_data/opti "\n\nRunning Standalone tests."
[ ${RUN_C_TEST} -ne 1 ] || run_test testC_smi         $DIR/test_data/opti "\n\nRunning SMI tests."

if [ "$TEST_STANDALONE_ONLY" = "1" ]; then exit 0; fi

for i in $DIR/test_data/opti $DIR/system_dumps/* ${UNIT_TEST_DATA_DIR}/platform/*; do
    [ -e $i ] || continue
[ ${RUN_C_TEST} -ne 1 ] || run_test testC_token  $i "\n\nRunning TOKEN test for $i"
[ ${RUN_C_TEST} -ne 1 ] || run_test testC_smbios $i "\n\nRunning SMBIOS test for $i"
[ ${RUN_CPP_TEST:=1} -ne 1 ] || run_test testPlatform $i "\n\nRunning PLATFORM test for $i"
done
